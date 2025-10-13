#include "clocks.h"
#include <Wire.h>
#include "time_utils.h"
#include "globals.h"

// sensor instances
AS5600 SensorS1(&I2C_channel_1);
AS5600 SensorS2(&I2C_channel_2);

Clock Clock1((int)1, H1_IN1, H1_IN2, H1_IN3, H1_IN4, HALL_10_PIN,
             M1_IN1, M1_IN2, M1_IN3, M1_IN4, HALL_11_PIN,
             S1_STEP, S1_DIR, SensorS1, SERVO_1);

Clock Clock2((int)2, H2_IN1, H2_IN2, H2_IN3, H2_IN4, HALL_20_PIN,
             M2_IN1, M2_IN2, M2_IN3, M2_IN4, HALL_21_PIN,
             S2_STEP, S2_DIR, SensorS2, SERVO_2);

// Constructor
Clock::Clock(int clockNumber,
             int in1H, int in2H, int in3H, int in4H, int sensorHPin,
             int in1M, int in2M, int in3M, int in4M, int sensorMPin,
             int STEPS, int DIRS, AS5600 &sensorS, int servoPin)
    : clockNumber(clockNumber),
      H(AccelStepper::HALF4WIRE, in4H, in2H, in3H, in1H),
      M(AccelStepper::HALF4WIRE, in4M, in2M, in3M, in1M),
      S(AccelStepper::DRIVER, STEPS, DIRS),
      sensorHPin(sensorHPin),
      sensorMPin(sensorMPin),
      sensorS(sensorS),
      servoPin(servoPin)
{
    targetPositions[0] = 0;
    targetPositions[1] = 0;
    targetPositions[2] = 0;
}

void Clock::begin()
{
    initSensors();
    initSteppers();
}

void Clock::initSensors()
{
    pinMode(sensorHPin, INPUT);
    pinMode(sensorMPin, INPUT);

    I2C_channel_1.begin(HALL_12_SDA, HALL_12_SCL); // SDA, SCL
    I2C_channel_2.begin(HALL_22_SDA, HALL_22_SCL);

    SensorS1.begin();
    SensorS2.begin();
    if (SensorS1.begin())
    {
        Serial.println("S1 AS5600 Found!");
    }
    else
    {
        Serial.println("S1 AS5600 Not Found. Check wiring.");
        while (1)
            ; // Stop here
    }

    if (SensorS2.begin())
    {
        Serial.println("S2 AS5600 Found!");
    }
    else
    {
        Serial.println("S2 AS5600 Not Found. Check wiring.");
        while (1)
            ; // Stop here
    }
}

void Clock::initSteppers()
{
    multi.addStepper(H);
    multi.addStepper(M);
    multi.addStepper(S);

    S.setMinPulseWidth(20);  // <-- CRITICAL for TMC2209
}

void Clock::home()
{
    sensorMState = false;
    sensorHState = false;

    M.setMaxSpeed(1000.0); // speed for homing
    H.setMaxSpeed(1000.0);

    M.setSpeed(HMhomingSpeed);
    H.setSpeed(HMhomingSpeed);

    bool preRunM = digitalRead(sensorMPin);
    bool preRunH = digitalRead(sensorHPin);

    unsigned long startTime = millis(); // record the start time

    // Run until both sensors are triggered
    while (!sensorMState || !sensorHState)
    {
        if (preRunM)
        {
            M.runSpeed();
            if (millis() - startTime > 5000)
            {
                preRunM = false;
            }
        }
        else if (!sensorMState)
        {
            sensorMState = digitalRead(sensorMPin);
            if (!sensorMState)
                M.runSpeed();
            else
                M.disableOutputs(); // stop holding power
        }

        if (preRunH)
        {
            H.runSpeed();
            if (millis() - startTime > 5000)
            {
                preRunH = false;
            }
        }
        else if (!sensorHState)
        {
            sensorHState = digitalRead(sensorHPin);
            if (!sensorHState)
                H.runSpeed();
            else
                H.disableOutputs(); // stop holding power
        }

        // Avoid blocking too long without yielding
        delay(1);
    }

    // Stop both motors completely
    M.setSpeed(0);
    H.setSpeed(0);
    // Serial.println("setting motors H and M current position to 0");
    M.setCurrentPosition(0);
    H.setCurrentPosition(0);

    // Serial.println("calibrating seccond hand");
    calibrateSecondHand();

    Serial.println("Homing complete.");
}

void Clock::calibrateSecondHand()
{
    // Serial.println("reading SensorS sensor value");
    int sensorValue = sensorS.readAngle();
    int steps = (int)(sensorValue * (SstepsPerTurn / 4096.0)) % SstepsPerTurn;
    if (steps < 0)
        steps += SstepsPerTurn;
    // Serial.println("setting S current position");
    S.setCurrentPosition(steps);
}

void Clock::setTimeSpeed()
{
    S.setMaxSpeed((float)SstepsPerTurn / 60.0 + 100);
    S.setSpeed((float)-SstepsPerTurn / 60.0);
    S.enableOutputs();

    M.setMaxSpeed((float)HMstepsPerTurn / 3600.0 + 100);
    M.setSpeed((float)HMstepsPerTurn / 3600.0);
    M.enableOutputs();

    H.setMaxSpeed((float)HMstepsPerTurn / 43200.0 + 100);
    H.setSpeed((float)HMstepsPerTurn / 43200.0);
    H.enableOutputs();
}

void Clock::setModeChangeSpeed()
{
    S.setMaxSpeed(SmodeChangeSpeed);
    S.setAcceleration(SmodeChangeAccel);
    S.enableOutputs();

    M.setMaxSpeed(HMmodeChangeSpeed);
    M.setAcceleration(HMmodeChangeAccel);
    M.enableOutputs();

    H.setMaxSpeed(HMmodeChangeSpeed);
    H.setAcceleration(HMmodeChangeAccel);
    H.enableOutputs();
}

void Clock::runTimeMode()
{
    // unsigned long currentMillis = millis();

    // Always run S and M
    S.runSpeed();
    M.runSpeed();
    H.runSpeed();

    // // Check if we should toggle H
    // if (!h1Active && currentMillis - previousHMillis >= intervalHActive)
    // {
    //     // Activate H1
    //     h1Active = true;
    //     previousHMillis = currentMillis;
    // }

    // if (h1Active)
    // {
    //     H.runSpeed(); // Run H while active

    //     // Deactivate H1 after intervalHActive milliseconds
    //     if (currentMillis - previousHMillis >= intervalHActive)
    //     {
    //         h1Active = false;
    //         previousHMillis = currentMillis; // reset timer for next cycle
    //     }
    // }
}

void Clock::runMultiMode()
{
    if (H.targetPosition() != targetPositions[0] || M.targetPosition() != targetPositions[1] || S.targetPosition() != targetPositions[2])
    {
        multi.moveTo(targetPositions);
    }
    multi.run();
}

void Clock::setTargetPositions(long hourSteps, long minuteSteps, long secondSteps, int currentHPosition, int currentMPosition, int currentSPosition)
{
    targetPositions[0] = wrapTarget(currentHPosition, targetPositions[0], HMstepsPerTurn);
    targetPositions[1] = wrapTarget(currentMPosition, targetPositions[1], HMstepsPerTurn);
    targetPositions[2] = wrapTarget(currentSPosition, targetPositions[2], SstepsPerTurn);
}

void Clock::computeAndSetTargetsFromTime(int hour, int minute, int second, int currentHPosition, int currentMPosition, int currentSPosition)
{
    second += TIME_FOR_GOING_TO_TIME_POSITION / 1000;
    if (second > 60)
    {
        second -= 60;
        minute += 1;
    }
    if (minute > 60)
    {
        minute -= 60;
        hour += 1;
    }

    targetPositions[0] = wrapTarget(currentHPosition, (int)(hour * hourStepsPerHour + minute * hourStepsPerMinute + second * hourStepsPerSecond) % HMstepsPerTurn, HMstepsPerTurn);
    targetPositions[1] = wrapTarget(currentMPosition, (int)(minute * minuteStepsPerMinute + second * minuteStepsPerSecond) % HMstepsPerTurn, HMstepsPerTurn);
    targetPositions[2] = wrapTarget(currentSPosition, (int)(second * secondStepsPerSecond) % SstepsPerTurn, SstepsPerTurn);
}

void Clock::moveToTime(int hour, int minute, int second)
{
    computeAndSetTargetsFromTime(hour, minute, second, H.currentPosition(), M.currentPosition(), S.currentPosition());
    while (H.distanceToGo() != 0 || M.distanceToGo() != 0 || S.distanceToGo() != 0)
    {
        runMultiMode();
    }
}

void Clock::getAndMoveToTime()
{
    Serial.println("getAndMoveToTime() start");
    GetLocalTimeSafe(timeinfo);

    Serial.println("--Reading time...");
    lastUpdatedTimeData = millis();

    Serial.printf("--Time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    Serial.println("--Computing step targets...");
    computeAndSetTargetsFromTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, H.currentPosition(), M.currentPosition(), S.currentPosition());

    setModeChangeSpeed();

    Serial.println("--Moving to targets...");
    while (millis() - lastUpdatedTimeData < TIME_FOR_GOING_TO_TIME_POSITION)
    {
        runMultiMode();
        delay(1);
    }

    Serial.println("getAndMoveToTime() done");
}

void Clock::normalizeStepCounts() // use everytime a stepper step count goes more than StepsPerTurn
{
    S.setCurrentPosition(Normalize(S.currentPosition(), HMstepsPerTurn));
    M.setCurrentPosition(Normalize(M.currentPosition(), HMstepsPerTurn));
    H.setCurrentPosition(Normalize(H.currentPosition(), SstepsPerTurn));

    // ! also normalize the target position

    targetPositions[0] = Normalize(targetPositions[0], HMstepsPerTurn);
    targetPositions[1] = Normalize(targetPositions[1], HMstepsPerTurn);
    targetPositions[2] = Normalize(targetPositions[2], SstepsPerTurn);
}

int Clock::Normalize(int pos, int stepsPerTurn) // returns a position between 0 and stepsPerTurn
{
    pos = pos % stepsPerTurn;
    while (pos > stepsPerTurn)
        pos -= stepsPerTurn;
    while (pos < 0)
        pos += stepsPerTurn;
    return pos;
}

void Clock::printStatus()
{
    Serial.printf("[%s] H:%ld  M:%ld  S:%ld\n", clockNumber, H.currentPosition(), M.currentPosition(), S.currentPosition());
}

long *Clock::getTargetArray()
{
    return targetPositions;
}

int Clock::wrapTarget(int currentPosition, int target, int stepsPerTurn)
{
    int delta = target - currentPosition;
    while (delta > stepsPerTurn / 2)
    {
        target -= stepsPerTurn;
        delta = target - currentPosition;
    }
    while (delta < -stepsPerTurn / 2)
    {
        target += stepsPerTurn;
        delta = target - currentPosition;
    }
    return target;
}
