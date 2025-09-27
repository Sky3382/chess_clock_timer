#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

// Timestamps
extern unsigned long loopTimestamp;
extern unsigned long lastHourHandMovement;
extern unsigned long lastUpdatedTimeData;
extern unsigned long lastUpdatedSteps;
extern unsigned long lastUpdatedHour;

// Modes
extern int mode;

// Hour-hand timing control
extern unsigned long previousHMillis;
extern bool hActive;
extern const unsigned long intervalHActive;

#endif