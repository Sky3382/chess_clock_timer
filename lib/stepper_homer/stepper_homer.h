#pragma once
#include <AccelStepper.h>
#include <Arduino.h>

class StepperHomer
{
public:
    StepperHomer(AccelStepper *stepper, int sensorPin, int speed);
    void begin();
    void update();
    bool isDone() const;

private:
    AccelStepper *stepper;
    int sensorPin;
    int speed;
    bool preRun;
    int state;
    unsigned long startTime;
    bool done;
};
