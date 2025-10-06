#include <Arduino.h>
#include <Wire.h>
#include "steppers.h"
#include "time_utils.h"
#include "globals.h"
#include "AS5600.h"
#include "stepper_homer.h"

// Configuration Variables
int homingSpeed = 1000;
int modeChangeSpeed = 1000;
int modeChangeAcceleration = 200;
int adjust_hands_speed = 12000; // time in ms to move hands to correct time

// speeds
//  Move one full turn in 60 seconds for S, 3600 seconds for M, 43200 seconds for H
float S_speed = (float)STEPS_PER_TURN / 60.0;    // steps per second for second hand
float M_speed = (float)STEPS_PER_TURN / 3600.0;  // steps per second for minute hand
float H_speed = (float)STEPS_PER_TURN / 43200.0; // steps per second for hour hand

// Instantiate steppers (HALF4WIRE)
AccelStepper S1(AccelStepper::HALF4WIRE, in1PinS1, in3PinS1, in2PinS1, in4PinS1);
AccelStepper M1(AccelStepper::HALF4WIRE, in1PinM1, in3PinM1, in2PinM1, in4PinM1);
AccelStepper H1(AccelStepper::HALF4WIRE, in1PinH1, in3PinH1, in2PinH1, in4PinH1);
MultiStepper Clock1;

AccelStepper S2(AccelStepper::HALF4WIRE, in1PinS2, in3PinS2, in2PinS2, in4PinS2);
AccelStepper M2(AccelStepper::HALF4WIRE, in1PinM2, in3PinM2, in2PinM2, in4PinM2);
AccelStepper H2(AccelStepper::HALF4WIRE, in1PinH2, in3PinH2, in2PinH2, in4PinH2);
MultiStepper Clock2;

StepperHomer homerM1(&M1, SensorM1, homingSpeed);
StepperHomer homerH1(&H1, SensorH1, homingSpeed);
StepperHomer homerM2(&M2, SensorM2, homingSpeed);
StepperHomer homerH2(&H2, SensorH2, homingSpeed);

long clock1TargetPositions[3] = {0, 0, 0};
long clock2TargetPositions[3] = {0, 0, 0};

AS5600 SensorS1;
AS5600 SensorS2;

const int stepsPerTurn = 20480;
const double hourStepsPerHour = (double)stepsPerTurn / 12.0;
const double hourStepsPerMinute = hourStepsPerHour / 60.0;
const double hourStepsPerSecond = hourStepsPerMinute / 60.0;

const double minuteStepsPerMinute = stepsPerTurn / 60.0;
const double minuteStepsPerSecond = minuteStepsPerMinute / 60.0;

const double secondStepsPerSecond = stepsPerTurn / 60.0;

void InitClockSteppers1()
{
    // add to multistepper in same order as target array
    Clock1.addStepper(H1);
    Clock1.addStepper(M1);
    Clock1.addStepper(S1);
}

void InitClockSteppers2()
{
    // add to multistepper in same order as target array
    Clock2.addStepper(H2);
    Clock2.addStepper(M2);
    Clock2.addStepper(S2);
}

void InitSensors1()
{
    pinMode(SensorM1, INPUT);
    pinMode(SensorH1, INPUT);

    Wire.begin(); // SDA, SCL

    if (SensorS1.begin())
    {
        Serial.println("AS5600 Found!");
    }
    else
    {
        Serial.println("AS5600 Not Found. Check wiring.");
        while (1)
            ; // Stop here
    }
}

void InitSensors2()
{
    pinMode(SensorM2, INPUT);
    pinMode(SensorH2, INPUT);

    Wire.begin();

    if (SensorS2.begin())
    {
        Serial.println("AS5600 Found!");
    }
    else
    {
        Serial.println("AS5600 Not Found. Check wiring.");
        while (1)
            ; // Stop here
    }
}

void Homing1()
{
    bool SensorM1State = false;
    bool SensorH1State = false;

    // Ensure motors are moving slowly for accurate stopping
    M1.setMaxSpeed(1000.0); // reduce speed for homing
    H1.setMaxSpeed(1000.0);

    M1.setSpeed(homingSpeed); // negative or positive depends on wiring
    H1.setSpeed(homingSpeed);

    bool preRunM = digitalRead(SensorM1);
    bool preRunH = digitalRead(SensorH1);

    unsigned long startTime = millis(); // record the start time

    // Run until both sensors are triggered
    while (!SensorM1State || !SensorH1State)
    {
        if (preRunM)
        {
            M1.runSpeed();
            if (millis() - startTime > 5000)
            {
                preRunM = false;
            }
        }
        else if (!SensorM1State)
        {
            SensorM1State = digitalRead(SensorM1);
            if (!SensorM1State)
                M1.runSpeed();
            else
                M1.disableOutputs(); // stop holding power
        }

        if (preRunH)
        {
            H1.runSpeed();
            if (millis() - startTime > 5000)
            {
                preRunH = false;
            }
        }
        else if (!SensorH1State)
        {
            SensorH1State = digitalRead(SensorH1);
            if (!SensorH1State)
                H1.runSpeed();
            else
                H1.disableOutputs(); // stop holding power
        }

        // Avoid blocking too long without yielding
        delay(1);
    }

    // Stop both motors completely
    M1.setSpeed(0);
    H1.setSpeed(0);
    M1.setCurrentPosition(0);
    H1.setCurrentPosition(0);

    calibrateSecondHand(S1, SensorS1);
}

void Homing2()
{
    bool SensorM2State = false;
    bool SensorH2State = false;

    // Ensure motors are moving slowly for accurate stopping
    M2.setMaxSpeed(1000.0); // reduce speed for homing
    H2.setMaxSpeed(1000.0);

    M2.setSpeed(homingSpeed); // negative or positive depends on wiring
    H2.setSpeed(homingSpeed);

    bool preRunM = digitalRead(SensorM2);
    bool preRunH = digitalRead(SensorH2);

    unsigned long startTime = millis(); // record the start time

    // Run until both sensors are triggered
    while (!SensorM2State || !SensorH2State)
    {
        if (preRunM)
        {
            M2.runSpeed();
            if (millis() - startTime > 5000)
            {
                preRunM = false;
            }
        }
        else if (!SensorM2State)
        {
            SensorM2State = digitalRead(SensorM2);
            if (!SensorM2State)
                M2.runSpeed();
            else
                M2.disableOutputs(); // stop holding power
        }

        if (preRunH)
        {
            H2.runSpeed();
            if (millis() - startTime > 5000)
            {
                preRunH = false;
            }
        }
        else if (!SensorH2State)
        {
            SensorH2State = digitalRead(SensorH2);
            if (!SensorH2State)
                H2.runSpeed();
            else
                H2.disableOutputs(); // stop holding power
        }

        // Avoid blocking too long without yielding
        delay(1);
    }

    // Stop both motors completely
    M2.setSpeed(0);
    H2.setSpeed(0);
    M2.setCurrentPosition(0);
    H2.setCurrentPosition(0);

    calibrateSecondHand(S2, SensorS2);
}

void calibrateSecondHand(AccelStepper &S, AS5600 &SensorS)
{
    S.setCurrentPosition((int)(SensorS.readAngle()) * (20480 / 4096) % STEPS_PER_TURN); // set S2 position based on sensor reading
    while (S.currentPosition() < 0)
        S.setCurrentPosition(S.currentPosition() + 20480); // ensure positive position
}

int StepsToMoveToRightHour(int currentPosition, int hour, int min)
{
    const int stepsPerTurn = 20480;
    const double stepsPerHour = (double)stepsPerTurn / 12.0; // 1706.666...
    const double stepsPerMinute = stepsPerHour / 60.0;       // ~28.444...

    // Normalize hour
    hour %= 12;

    // Compute target position within one revolution
    int targetBase = (int)(stepsPerHour * hour + stepsPerMinute * min);
    targetBase = (targetBase % stepsPerTurn + stepsPerTurn) % stepsPerTurn;

    // Normalize current position
    currentPosition = (currentPosition % stepsPerTurn + stepsPerTurn) % stepsPerTurn;

    // Compute shortest delta
    int delta = targetBase - currentPosition;
    while (delta >= stepsPerTurn / 2)
        delta -= stepsPerTurn;
    while (delta <= -stepsPerTurn / 2)
        delta += stepsPerTurn;

    int absoluteTarget = currentPosition + delta;

    Serial.printf("HOUR | Current: %d | TargetBase: %d | Delta: %d | AbsTarget: %d\n",
                  currentPosition, targetBase, delta, absoluteTarget);

    return absoluteTarget % STEPS_PER_TURN; // Absolute position, not wrapped
}

int StepsToMoveToRightMinute(int currentPosition, int min, int sec)
{
    const int stepsPerTurn = 20480;
    const double stepsPerMinute = (double)stepsPerTurn / 60.0; // 341.333...
    const double stepsPerSecond = stepsPerMinute / 60.0;       // ~5.6889

    // Normalize input (+10s offset)
    sec += adjust_hands_speed / 1000; // add offset to seconds for mechanical alignment
    if (sec >= 60)
    {
        sec -= 60;
        min++;
    }
    if (min >= 60)
        min -= 60;

    // Compute target position within one revolution
    int targetBase = (int)(stepsPerMinute * min + stepsPerSecond * sec);
    targetBase = (targetBase % stepsPerTurn + stepsPerTurn) % stepsPerTurn;

    // Normalize current position
    currentPosition = (currentPosition % stepsPerTurn + stepsPerTurn) % stepsPerTurn;

    // Compute shortest delta
    int delta = targetBase - currentPosition;
    while (delta >= stepsPerTurn / 2)
        delta -= stepsPerTurn;
    while (delta <= -stepsPerTurn / 2)
        delta += stepsPerTurn;

    int absoluteTarget = currentPosition + delta;

    Serial.printf("MINUTE | Current: %d | TargetBase: %d | Delta: %d | AbsTarget: %d\n",
                  currentPosition, targetBase, delta, absoluteTarget);

    return absoluteTarget % STEPS_PER_TURN; // Absolute position, not wrapped
}

int StepsToMoveToRightSecond(int currentSensorPosition, int sec)
{
    const int stepsPerTurn = 20480;
    const double stepsPerSecond = (double)stepsPerTurn / 60.0; // 341.333...

    // Convert sensor position (0–4096) → motor steps (0–20480)
    // Negative sign because direction is inverted
    int currentPosition = (int)(currentSensorPosition * (-(stepsPerTurn) / 4096.0));

    // Normalize to 0–stepsPerTurn
    currentPosition = (currentPosition % stepsPerTurn + stepsPerTurn) % stepsPerTurn;

    // Offset +12 seconds (for mechanical alignment)
    sec = (sec + (adjust_hands_speed / 1000)) % 60;

    // Target position in steps (0–stepsPerTurn)
    int targetPosition = (int)(stepsPerSecond * sec);

    // Compute raw delta
    int delta = targetPosition - currentPosition;

    // Normalize delta to range -stepsPerTurn/2 .. +stepsPerTurn/2
    while (delta >= stepsPerTurn / 2)
        delta -= stepsPerTurn;
    while (delta <= -stepsPerTurn / 2)
        delta += stepsPerTurn;

    // Return the ABSOLUTE step position (not modulo)
    int absoluteTarget = currentPosition + delta;

    Serial.printf("Current Pos: %d | Target Base: %d | Delta: %d | AbsTarget: %d\n",
                  currentPosition, targetPosition, delta, absoluteTarget);

    return absoluteTarget % STEPS_PER_TURN; // Absolute position, not wrapped
}

void MoveToRightTime1()
{
    SetModeChangeSpeed(S1, M1, H1);

    Clock1.moveTo(clock1TargetPositions);
    while (H1.distanceToGo() != 0 || M1.distanceToGo() != 0 || S1.distanceToGo() != 0)
    {
        Clock1.run();
    }
    lastHourHandMovement = millis();
}

void MoveToRightTime2()
{
    SetModeChangeSpeed(S2, M2, H2);

    Clock2.moveTo(clock2TargetPositions);
    while (H2.distanceToGo() != 0 || M2.distanceToGo() != 0 || S2.distanceToGo() != 0)
    {
        Clock2.run();
    }
    lastHourHandMovement = millis();
}

void SetTimeSpeed(AccelStepper &S, AccelStepper &M, AccelStepper &H)
{

    S.setMaxSpeed(S_speed);
    S.setSpeed(S_speed);
    S.enableOutputs();

    M.setMaxSpeed(M_speed);
    M.setSpeed(M_speed);
    M.enableOutputs();

    H.setMaxSpeed(H_speed);
    H.setSpeed(H_speed);
    H.enableOutputs();
}

void SetModeChangeSpeed(AccelStepper &S, AccelStepper &M, AccelStepper &H)
{
    H.setMaxSpeed(modeChangeSpeed);
    H.setAcceleration(modeChangeAcceleration);

    M.setMaxSpeed(modeChangeSpeed);
    M.setAcceleration(modeChangeAcceleration);

    S.setMaxSpeed(modeChangeSpeed);
    S.setAcceleration(modeChangeAcceleration);
}

void RunHandsTime1()
{
    unsigned long currentMillis = millis();

    // Always run S and M
    S1.runSpeed();
    S1.runSpeed();
    M1.runSpeed();

    // Check if we should toggle H
    if (!h1Active && currentMillis - previousHMillis >= intervalHActive)
    {
        // Activate H1
        h1Active = true;
        previousHMillis = currentMillis;
    }

    if (h1Active)
    {
        H1.runSpeed(); // Run H while active

        // Deactivate H1 after intervalHActive milliseconds
        if (currentMillis - previousHMillis >= intervalHActive)
        {
            h1Active = false;
            previousHMillis = currentMillis; // reset timer for next cycle
        }
    }
}

void RunHandsTime2()
{
    unsigned long currentMillis = millis();

    // Always run S and M
    S2.runSpeed();
    S2.runSpeed();
    M2.runSpeed();

    // Check if we should toggle H
    if (!h2Active && currentMillis - previousHMillis >= intervalHActive)
    {
        // Activate H2
        h2Active = true;
        previousHMillis = currentMillis;
    }

    if (h2Active)
    {
        H2.runSpeed(); // Run H while active

        // Deactivate H2 after intervalHActive milliseconds
        if (currentMillis - previousHMillis >= intervalHActive)
        {
            h2Active = false;
            previousHMillis = currentMillis; // reset timer for next cycle
        }
    }
}

void UpdateMotorsStepCount()
{
    //calibrateSecondHand(S2, SensorS2);
    auto normalize = [](long pos)
    {
        long half = STEPS_PER_TURN / 2;
        pos = pos % STEPS_PER_TURN;
        if (pos > half)
            pos -= STEPS_PER_TURN;
        else if (pos < -half)
            pos += STEPS_PER_TURN;
        return pos;
    };

    S1.setCurrentPosition(normalize(S1.currentPosition()));
    M1.setCurrentPosition(normalize(M1.currentPosition()));
    H1.setCurrentPosition(normalize(H1.currentPosition()));
    S2.setCurrentPosition(normalize(S2.currentPosition()));
    M2.setCurrentPosition(normalize(M2.currentPosition()));
    H2.setCurrentPosition(normalize(H2.currentPosition()));
}

void GetAndMoveToTime(bool clock1, bool clock2)
{
    //______Get Time______
    GetLocalTimeSafe(timeinfo);

    lastUpdatedTimeData = millis();

    Serial.printf("Time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    //______move to right time______
    clock1TargetPositions[0] = StepsToMoveToRightHour(0, timeinfo.tm_hour, timeinfo.tm_min);
    clock1TargetPositions[1] = StepsToMoveToRightMinute(0, timeinfo.tm_min, timeinfo.tm_sec);
    clock1TargetPositions[2] = StepsToMoveToRightSecond(SensorS1.readAngle(), timeinfo.tm_sec);

    if (clock1)
        MoveToRightTime1();
    if (clock2)
        MoveToRightTime2();
}

int NormalizeStep(int value)
{
    int halfTurn = STEPS_PER_TURN / 2;
    while (value >= halfTurn)
        value -= STEPS_PER_TURN;
    while (value < -halfTurn)
        value += STEPS_PER_TURN;
    return value;
}