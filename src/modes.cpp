#include "modes.h"
#include "steppers.h"
#include "globals.h"

int timeToHour = 0;
int timeToMinute = 0;
int timeToSecond = 0;
int lastTimeToHour = 0;
int lastTimeToMinute = 0;
int lastTimeToSecond = 0;

int timeToClock = 0;

void RunMode1()
{
    SetTimeSpeed(S1, M1, H1);
}

void RunMode2()
{
}
