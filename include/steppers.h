#ifndef STEPPERS_H
#define STEPPERS_H

#include <AccelStepper.h>
#include <MultiStepper.h>

// Pin definitions
#define in1PinS 13
#define in2PinS 12
#define in3PinS 14
#define in4PinS 27

#define in1PinM 26
#define in2PinM 25
#define in3PinM 33
#define in4PinM 32

#define in1PinH 16
#define in2PinH 17
#define in3PinH 18
#define in4PinH 19

// Sensor pins
#define SensorM 35
#define SensorH 34

// Externs for steppers and controller
extern AccelStepper S1;
extern AccelStepper M1;
extern AccelStepper H1;
extern MultiStepper Clock1;

// Targets arrays
extern long clock1TargetPositions[3];

// Configuration variables
extern int homingSpeed;
extern int modeChangeSpeed;
extern int modeChangeAcceleration;

// Timing globals
extern unsigned long lastHourHandMovement;
extern unsigned long lastUpdatedSteps;
extern unsigned long lastUpdatedTimeData;
extern unsigned long lastUpdatedHour;

// Functions
void InitSteppers();
void Homing();
int StepsToMoveToRightHour(int hour, int min);
int StepsToMoveToRightMinute(int min, int sec);
void MoveToRightTime();
void SetTimeSpeed();
void SetModeChangeSpeed();
void RunHandsTime();

#endif // STEPPERS_H