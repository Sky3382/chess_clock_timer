#include "steppers.h"
#include "time_utils.h"
#include "globals.h"
#include <Arduino.h>

// Instantiate steppers (HALF4WIRE)
AccelStepper S1(AccelStepper::HALF4WIRE, in1PinS1, in3PinS1, in2PinS1, in4PinS1);
AccelStepper M1(AccelStepper::HALF4WIRE, in1PinM1, in3PinM1, in2PinM1, in4PinM1);
AccelStepper H1(AccelStepper::HALF4WIRE, in1PinH1, in3PinH1, in2PinH1, in4PinH1);
MultiStepper Clock1;

AccelStepper S2(AccelStepper::HALF4WIRE, in1PinS2, in3PinS2, in2PinS2, in4PinS2);
AccelStepper M2(AccelStepper::HALF4WIRE, in1PinM2, in3PinM2, in2PinM2, in4PinM2);
AccelStepper H2(AccelStepper::HALF4WIRE, in1PinH2, in3PinH2, in2PinH2, in4PinH2);
MultiStepper Clock2;

long clock1TargetPositions[3] = {0, 0, 0};

int homingSpeed = 1000;
int modeChangeSpeed = 1000;
int modeChangeAcceleration = 300;

void InitSteppers1()
{
    // add to multistepper in same order as target array
    Clock1.addStepper(H1);
    Clock1.addStepper(M1);
    Clock1.addStepper(S1);
}

void InitSteppers2()
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
}

void InitSensors2()
{
    pinMode(SensorM2, INPUT);
    pinMode(SensorH2, INPUT);
}

void Homing1()
{
    bool SensorM1State = false;
    bool SensorH1State = false;

    // Ensure motors are moving slowly for accurate stopping
    M1.setMaxSpeed(1000.0); // reduce speed for homing
    H1.setMaxSpeed(1000.0);

    M1.setSpeed(-homingSpeed); // negative or positive depends on wiring
    H1.setSpeed(-homingSpeed);

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
}



void Homing2()
{
    bool SensorM2State = false;
    bool SensorH2State = false;

    // Ensure motors are moving slowly for accurate stopping
    M2.setMaxSpeed(1000.0); // reduce speed for homing
    H2.setMaxSpeed(1000.0);

    M2.setSpeed(-homingSpeed); // negative or positive depends on wiring
    H2.setSpeed(-homingSpeed);

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
}

int StepsToMoveToRightHour(int currentPosition, int hour, int min)
{
    int stepsToTake;
    hour = hour % 12;

    if (hour < 6)
    {
        stepsToTake = -(1706.67 * hour + (1706.67 / 60) * min);
    }
    else
    {
        stepsToTake = 20480 - (1706.67 * hour + (1706.67 / 60) * min);
    }

    return stepsToTake;
}

int StepsToMoveToRightMinute(int currentPosition, int min, int sec)
{
    int stepsToTake;
    sec = sec + 10;
    if (sec > 60)
    {
        sec -= 60;
        min++;
    }
    if (min < 30)
    {
        stepsToTake = -(341.33 * (min) + (341.33 / 60) * sec);
    }
    else
    {
        stepsToTake = 20480 - (341.33 * (min) + (341.33 / 60) * sec);
    }

    return stepsToTake;
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
    S.setMaxSpeed(800.0);
    S.setSpeed(-(341 + (2 / 3)));
    S.enableOutputs();

    M.setMaxSpeed(10.0);
    M.setSpeed(-5.69);
    M.enableOutputs();

    H.setMaxSpeed(800.0);
    H.setSpeed(-0.95);
    H.enableOutputs();
}

void SetModeChangeSpeed(AccelStepper &S, AccelStepper &M, AccelStepper &H)
{
    H.setMaxSpeed(modeChangeSpeed);
    M.setMaxSpeed(modeChangeSpeed);
    S.setMaxSpeed(modeChangeSpeed);

    H.setAcceleration(modeChangeAcceleration);
    M.setAcceleration(modeChangeAcceleration);
    S.setAcceleration(modeChangeAcceleration);
}

void RunHandsTime(AccelStepper &S, AccelStepper &M, AccelStepper &H)
{
    unsigned long currentMillis = millis();

    // Always run S1 and M1
    S.runSpeed();
    M.runSpeed();

    // Check if we should toggle H1
    if (!hActive && currentMillis - previousHMillis >= intervalHActive)
    {
        // Activate H1
        hActive = true;
        previousHMillis = currentMillis;
    }

    if (hActive)
    {
        H.runSpeed(); // Run H1 while active

        // Deactivate H1 after intervalHActive milliseconds
        if (currentMillis - previousHMillis >= intervalHActive)
        {
            hActive = false;
            previousHMillis = currentMillis; // reset timer for next cycle
        }
    }
}