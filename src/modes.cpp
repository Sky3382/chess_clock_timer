#include "modes.h"
#include "steppers.h"
#include "globals.h"

int timeToHour = 0;
int timeToMinute = 0;
int timeToSecond = 0;

void RunMode1()
{
    SetTimeSpeed(S1, M1, H1);
}

void RunMode2()
{
}
