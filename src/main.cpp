#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include "globals.h"
#include "steppers.h"
#include "modes.h"
#include "time_utils.h"
#include "wifi_setup.h"
#include "input.h"

void setup()
{
    Serial.begin(115200);

    //______Initialize wifi and time______

    Connect_To_Wifi();
    Serial.println("Connected to Wifi");

    // Initialize time
    InitTime();

    //______Initialize multimotors______
    SetModeChangeSpeed(S1, M1, H1);
    InitSteppers1();
    InitSensors1();
    // InitSteppers2();
    // InitSensors2();
    InitEncoder();
    InitButtons();

    //______Motors to zero______
    Homing1();
    // Homing2();

    // Move to the right time
    GetAndMoveToTime(true, false);

    while (millis() - lastUpdatedTimeData < 10000)
    {
        delay(1);
    }

    Serial.println("Clock on time");
    lastUpdatedSteps = millis();
    lastUpdatedHour = millis();
}

void loop()
{
    loopTimestamp = millis();

    switch (state)
    {
    case 0:           // is state is default / display time
        switch (mode) // execute mode functions
        {
        case 0: // Time mode
            if (lastMode != mode)
            {
                GetAndMoveToTime(true, false);
            }

            steppersMovingMethod = 0; // time mode
            RunMode1();
            break;
        case 1: // Time to mode
            clock1active = true;
            clock2active = true;
            steppersMovingMethod = 1; // multi stepper
            break;
        case 2: // Pomodoro timer mode
            clock1active = false;
            clock2active = true;
            steppersMovingMethod = 1; // individual run speed
            break;
        default:
            clock1active = true;
            clock2active = false;
            steppersMovingMethod = 0; // time mode
        }

    case 1: // mode change
        break;

    case 2: // settings
        break;
    }

    RunMode1();

    switch (steppersMovingMethod)
    {
    case 0:
        if (clock1active)
            RunHandsTime1();

        if (clock2active)
            RunHandsTime2();

        break;
    case 1:
        if (clock1active)
            Clock1.run();
        if (clock2active)
            Clock2.run();

    case 2:
        if (clock1active)
        {
            S1.runSpeed();
            M1.runSpeed();
            H1.runSpeed();
        }
        if (clock2active)
        {
            S2.runSpeed();
            M2.runSpeed();
            H2.runSpeed();
        }
        break;
    }

    if (loopTimestamp - lastUpdatedSteps > 60000)
    {
        UpdateMotorsStepCount();
    }

    static uint32_t lastTime = 0;

    //  set initial position
    // Serial.println(SensorS1.readAngle());
}