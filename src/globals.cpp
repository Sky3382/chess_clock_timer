#include "globals.h"

// Timestamps
unsigned long loopTimestamp = 0;
unsigned long lastHourHandMovement = 0;
unsigned long lastUpdatedTimeData = 0;
unsigned long lastUpdatedSteps = 0;
unsigned long lastUpdatedHour = 0;

// Modes
int mode = 0; // (O is time, 1 is time to, 2 is pomodoro timer)
int lastMode = 0;
int state = 0; //(0 is normal, 1 is settings and 2 is calibration)
int lastState = 0;
int selectedMode = 0;
int steppersMovingMethod = 0; // 0 is time mode, 1 is multi stepper (target position), 2 is individual run speed

// Clcok states
bool clock1active = true;
bool clock2active = false;

// Hour-hand timing control
unsigned long previousHMillis = 0;
bool h1Active = false;
bool h2Active = false;
const unsigned long intervalHActive = 50000; // 50 seconds
