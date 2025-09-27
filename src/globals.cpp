#include "globals.h"

// Timestamps
unsigned long loopTimestamp = 0;
unsigned long lastHourHandMovement = 0;
unsigned long lastUpdatedTimeData = 0;
unsigned long lastUpdatedSteps = 0;
unsigned long lastUpdatedHour = 0;

// Modes
int mode = 0;

// Hour-hand timing control
unsigned long previousHMillis = 0;
bool hActive = false;
const unsigned long intervalHActive = 50000; // 50 seconds in ms