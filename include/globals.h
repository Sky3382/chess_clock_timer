#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Wire.h>

#define STEPS_PER_TURN 20480 // 28BYJ-48 with 1/64 reduction

#define DISPLAY_STATE 0
#define MODE_CHANGE_STATE 1
#define SETTINGS_STATE 2

#define TIME_MODE 0
#define TIME_TO_MODE 1
#define POMODORO_MODE 2

//I2C channels
extern TwoWire I2C_channel_1;
extern TwoWire I2C_channel_2;

// Timestamps
extern unsigned long loopTimestamp;
extern unsigned long lastHourHandMovement;
extern unsigned long lastUpdatedTimeData;
extern unsigned long lastUpdatedSteps;
extern unsigned long lastUpdatedHour;

// Modes
extern int mode; // (O is time, 1 is time to, 2 is pomodoro timer)
extern int lastMode;
extern int state; //(0 is normal, 1 for mode change, 2 is settings and 3 is calibration)
extern int last_state;
extern int selected_mode;
extern int steppers_moving_method; // 0 is time mode, 1 is multi stepper (target position), 2 is individual run speed

extern bool isClock1active;
extern bool isClock2active;

extern int selected_clock;        // which clock is being adjusted in settings mode
extern int previous_selected_hand; // which hand was previously adjusted in settings mode
extern int selected_Clock1_hand; // which hand is being adjusted in settings mode
extern int selected_Clock2_hand; // which hand is being adjusted in settings mode

// Hour-hand timing control
extern bool h1Active;
extern bool h2Active;
extern const unsigned long intervalHActive; // 50 seconds in ms

#endif
