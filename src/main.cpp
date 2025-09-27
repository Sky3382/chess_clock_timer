#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include "globals.h"
#include "steppers.h"
#include "modes.h"
#include "time_utils.h"
#include "wifi_setup.h"

void setup()
{
    Serial.begin(115200);

    //______Initialize wifi and time______

    Connect_To_Wifi();
    Serial.println("Connected to Wifi");

    // Initialize time
    InitTime();

    //______Initialize multimotors______
    SetModeChangeSpeed();
    Clock1.addStepper(H1);
    Clock1.addStepper(M1);
    Clock1.addStepper(S1);

    //______Initialize Sensors______
    pinMode(SensorM, INPUT);
    pinMode(SensorH, INPUT);

    //______Motors to zero______
    Homing();

    //______Get Time______
    GetLocalTimeSafe(timeinfo);

    lastUpdatedTimeData = millis();

    Serial.printf("Time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    //______move to right time______
    clock1TargetPositions[0] = StepsToMoveToRightHour(timeinfo.tm_hour, timeinfo.tm_min);
    clock1TargetPositions[1] = StepsToMoveToRightMinute(timeinfo.tm_min, timeinfo.tm_sec);
    clock1TargetPositions[2] = 3000; // change later for the seconds

    MoveToRightTime();

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

    if (mode == 0)
    {
        SetTimeSpeed();
        RunHandsTime();
    }

    if (loopTimestamp - lastUpdatedSteps > 60000)
    {
        // S1.currentPosition() = S1.currentPosition() % 4096;
        M1.setCurrentPosition(M1.currentPosition() % 4096);
        H1.setCurrentPosition(H1.currentPosition() % 4096);
        lastUpdatedSteps = millis();
        // Serial.println(S1.currentPosition());
        Serial.println(M1.currentPosition());
        Serial.println(H1.currentPosition());
    }
}