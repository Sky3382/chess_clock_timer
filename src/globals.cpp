#include "globals.h"
#include <Wire.h>

//I2C channels
TwoWire I2C_channel_1 = TwoWire(0);
TwoWire I2C_channel_2 = TwoWire(1);

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

int selectedClock = 0;        // which clock is being adjusted in settings mode
int previousSelectedHand = 0; // which hand was previously adjusted in settings mode
int selectedHand = 0;         // which hand is being adjusted in settings mode (0 : hour, 1 : minute, 2 : second)

// Hour-hand timing control
unsigned long previousHMillis = 0;
bool h1Active = false;
bool h2Active = false;
const unsigned long intervalHActive = 50000; // 50 seconds
