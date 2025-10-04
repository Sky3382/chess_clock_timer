#ifndef STEPPERS_H
#define STEPPERS_H

#include <AccelStepper.h>
#include <MultiStepper.h>
#include <AS5600.h>

// Pin definitions
#define in1PinS1 13
#define in2PinS1 12
#define in3PinS1 14
#define in4PinS1 27

#define in1PinM1 26
#define in2PinM1 25
#define in3PinM1 33
#define in4PinM1 32

#define in1PinH1 16
#define in2PinH1 17
#define in3PinH1 18
#define in4PinH1 19

#define in1PinS2 13
#define in2PinS2 12
#define in3PinS2 14
#define in4PinS2 27

#define in1PinM2 26
#define in2PinM2 25
#define in3PinM2 33
#define in4PinM2 32

#define in1PinH2 16
#define in2PinH2 17
#define in3PinH2 18
#define in4PinH2 19

// Sensor pins
#define SensorM1 35
#define SensorH1 34
extern AS5600 SensorS1;

#define SensorM2 35
#define SensorH2 34
extern AS5600 SensorS2;

// Configuration variables
extern int homingSpeed;
extern int modeChangeSpeed;
extern int modeChangeAcceleration;

// Variables
extern int HSpeedTime;   // Hour hand speed in time mode (degrees per minute)
extern int MSpeedTime;   // Minute hand speed in time mode (degrees per minute)
extern int SSpeedTime; // Second hand speed in time mode (degrees per minute)




// Externs for steppers and controller
extern AccelStepper S1;
extern AccelStepper M1;
extern AccelStepper H1;
extern MultiStepper Clock1;

extern AccelStepper S2;
extern AccelStepper M2;
extern AccelStepper H2;
extern MultiStepper Clock2;

// Targets arrays
extern long clock1TargetPositions[3];
extern long clock2TargetPositions[3];

// Timing globals
extern unsigned long lastHourHandMovement;
extern unsigned long lastUpdatedSteps;
extern unsigned long lastUpdatedTimeData;
extern unsigned long lastUpdatedHour;

// Functions
void InitSteppers1();
void InitSteppers2();
void InitSensors1();
void InitSensors2();
void Homing1();
void Homing2();
int StepsToMoveToRightHour(int currentPosition, int hour, int min);
int StepsToMoveToRightMinute(int currentPosition, int min, int sec);
int StepsToMoveToRightSecond(int currentPosition, int sec);
void MoveToRightTime1();
void MoveToRightTime2();
void SetTimeSpeed(AccelStepper &S, AccelStepper &M, AccelStepper &H);
void SetModeChangeSpeed(AccelStepper &S, AccelStepper &M, AccelStepper &H);
void RunHandsTime1();
void RunHandsTime2();
void UpdateMotorsStepCount();
void GetAndMoveToTime(bool clock1, bool clock2);

#endif // STEPPERS_H