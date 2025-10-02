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

    //______Get Time______
    GetLocalTimeSafe(timeinfo);

    lastUpdatedTimeData = millis();

    Serial.printf("Time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    //______move to right time______
    clock1TargetPositions[0] = StepsToMoveToRightHour(0, timeinfo.tm_hour, timeinfo.tm_min);
    clock1TargetPositions[1] = StepsToMoveToRightMinute(0, timeinfo.tm_min, timeinfo.tm_sec);
    clock1TargetPositions[2] = StepsToMoveToRightSecond(SensorS1.readAngle(), timeinfo.tm_sec);

    MoveToRightTime1();

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

    RunMode();

    if (loopTimestamp - lastUpdatedSteps > 60000)
    {
        UpdateMotorsStepCount();
    }

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

    static uint32_t lastTime = 0;

    //  set initial position
    // Serial.println(SensorS1.readAngle());
}