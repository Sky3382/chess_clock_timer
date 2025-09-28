#include "modes.h"
#include "steppers.h"

extern int mode; // declared/defined in steppers.cpp

void RunMode()
{
    if (mode == 0)
    {
        SetTimeSpeed(S1, M1, H1);
        RunHandsTime(S1, M1, H1);

        // periodic housekeeping: normalize positions every minute
        static unsigned long lastUpdatedSteps_local = 0;
        unsigned long now = millis();
        if (now - lastUpdatedSteps_local > 60000)
        {
            M1.setCurrentPosition(M1.currentPosition() % 4096);
            H1.setCurrentPosition(H1.currentPosition() % 4096);
            lastUpdatedSteps_local = now;
            Serial.println(M1.currentPosition());
            Serial.println(H1.currentPosition());
        }
    }
}