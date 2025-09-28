#include "modes.h"
#include "steppers.h"
#include "globals.h"


void RunMode()
{
    if (mode == 0)
    {
        SetTimeSpeed(S1, M1, H1);
        RunHandsTime1();
        // RunHandsTime2();


    }
}