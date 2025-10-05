#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

#define STEPS_PER_TURN 20480 // 28BYJ-48 with 1/64 reduction

// Timestamps
extern unsigned long loopTimestamp;
extern unsigned long lastHourHandMovement;
extern unsigned long lastUpdatedTimeData;
extern unsigned long lastUpdatedSteps;
extern unsigned long lastUpdatedHour;

// Modes
extern int mode; // (O is time, 1 is time to, 2 is pomodoro timer)
extern int lastMode;
extern int state;                //(0 is normal, 1 for mode change, 2 is settings and 3 is calibration)
extern int lastState;
extern int selectedMode;
extern int steppersMovingMethod; // 0 is time mode, 1 is multi stepper (target position), 2 is individual run speed

extern bool clock1active;
extern bool clock2active;

extern int selectedClock; // which clock is being adjusted in settings mode
extern int selectedHand;  // which hand is being adjusted in settings mode

// Hour-hand timing control
extern unsigned long previousHMillis;
extern bool h1Active;
extern bool h2Active;
extern const unsigned long intervalHActive; // 50 seconds in ms

#endif
