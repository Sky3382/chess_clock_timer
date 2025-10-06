#include "stepper_homer.h"
#include <AccelStepper.h>
#include <Arduino.h>

// Constructor
StepperHomer::StepperHomer(AccelStepper *stepper, int sensorPin, int speed)
    : stepper(stepper), sensorPin(sensorPin), speed(speed), state(0), startTime(0), done(false) {}

// Begin homing
void StepperHomer::begin()
{
    stepper->setMaxSpeed(1000.0);
    stepper->setSpeed(speed);
    preRun = digitalRead(sensorPin);
    startTime = millis();
    state = 0;
    done = false;
}

// Update homing state
void StepperHomer::update()
{
    if (done)
        return;

    switch (state)
    {
    case 0: // Pre-run
        if (preRun)
        {
            stepper->runSpeed();
            if (millis() - startTime > 5000)
            {
                preRun = false;
                state = 1;
            }
        }
        else
        {
            state = 1;
        }
        break;
    case 1: // Homing
        if (!digitalRead(sensorPin))
        {
            stepper->runSpeed();
        }
        else
        {
            stepper->disableOutputs();
            done = true;
        }
        break;
    }
}

bool StepperHomer::isDone() const { return done; }