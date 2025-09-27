#include "steppers.h"
#include "time_utils.h"
#include "globals.h"
#include <Arduino.h>

// Instantiate steppers (HALF4WIRE)
AccelStepper S1(AccelStepper::HALF4WIRE, in1PinS, in3PinS, in2PinS, in4PinS);
AccelStepper M1(AccelStepper::HALF4WIRE, in1PinM, in3PinM, in2PinM, in4PinM);
AccelStepper H1(AccelStepper::HALF4WIRE, in1PinH, in3PinH, in2PinH, in4PinH);
MultiStepper Clock1;

AccelStepper S2(AccelStepper::HALF4WIRE, in1PinS, in3PinS, in2PinS, in4PinS);
AccelStepper M2(AccelStepper::HALF4WIRE, in1PinM, in3PinM, in2PinM, in4PinM);
AccelStepper H2(AccelStepper::HALF4WIRE, in1PinH, in3PinH, in2PinH, in4PinH);
MultiStepper Clock2;

long clock1TargetPositions[3] = {0, 0, 0};

int homingSpeed = 1000;
int modeChangeSpeed = 1000;
int modeChangeAcceleration = 300;

void InitSteppers()
{
    // add to multistepper in same order as target array
    Clock1.addStepper(H1);
    Clock1.addStepper(M1);
    Clock1.addStepper(S1);

    pinMode(SensorM, INPUT);
    pinMode(SensorH, INPUT);

    SetModeChangeSpeed();
}

void Homing()
{
    bool sensorMState = false;
    bool sensorHState = false;

    // Ensure motors are moving slowly for accurate stopping
    M1.setMaxSpeed(1000.0); // reduce speed for homing
    H1.setMaxSpeed(1000.0);

    M1.setSpeed(-homingSpeed); // negative or positive depends on wiring
    H1.setSpeed(-homingSpeed);

    bool preRunM = digitalRead(SensorM);
    bool preRunH = digitalRead(SensorH);

    unsigned long startTime = millis(); // record the start time

    // Run until both sensors are triggered
    while (!sensorMState || !sensorHState)
    {
        if (preRunM)
        {
            M1.runSpeed();
            if (millis() - startTime > 5000)
            {
                preRunM = false;
            }
        }
        else if (!sensorMState)
        {
            sensorMState = digitalRead(SensorM);
            if (!sensorMState)
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
        else if (!sensorHState)
        {
            sensorHState = digitalRead(SensorH);
            if (!sensorHState)
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

int StepsToMoveToRightHour(int hour, int min)
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

int StepsToMoveToRightMinute(int min, int sec)
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

void MoveToRightTime()
{
    SetModeChangeSpeed();

    Clock1.moveTo(clock1TargetPositions);
    while (H1.distanceToGo() != 0 || M1.distanceToGo() != 0 || S1.distanceToGo() != 0)
    {
        Clock1.run();
    }
    lastHourHandMovement = millis();
}

void SetTimeSpeed()
{
    S1.setMaxSpeed(800.0);
    S1.setSpeed(-(341 + (2 / 3)));
    S1.enableOutputs();

    M1.setMaxSpeed(10.0);
    M1.setSpeed(-5.69);
    M1.enableOutputs();

    H1.setMaxSpeed(800.0);
    H1.setSpeed(-0.95);
    H1.enableOutputs();
}

void SetModeChangeSpeed()
{
    H1.setMaxSpeed(modeChangeSpeed);
    M1.setMaxSpeed(modeChangeSpeed);
    S1.setMaxSpeed(modeChangeSpeed);

    H1.setAcceleration(modeChangeAcceleration);
    M1.setAcceleration(modeChangeAcceleration);
    S1.setAcceleration(modeChangeAcceleration);
}

void RunHandsTime()
{
    unsigned long currentMillis = millis();

    // Always run S1 and M1
    S1.runSpeed();
    M1.runSpeed();

    // Check if we should toggle H1
    if (!hActive && currentMillis - previousHMillis >= intervalHActive)
    {
        // Activate H1
        hActive = true;
        previousHMillis = currentMillis;
    }

    if (hActive)
    {
        H1.runSpeed(); // Run H1 while active

        // Deactivate H1 after intervalHActive milliseconds
        if (currentMillis - previousHMillis >= intervalHActive)
        {
            hActive = false;
            previousHMillis = currentMillis; // reset timer for next cycle
        }
    }
}