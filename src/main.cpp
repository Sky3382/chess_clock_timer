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
    Serial.println("Starting up...");

    //______Initialize wifi and time______

    Connect_To_Wifi();
    Serial.println("Connected to Wifi");

    // Initialize time
    InitTime();

    //______Initialize multimotors______
    SetModeChangeSpeed(S1, M1, H1);
    InitClockSteppers1();
    homerM1.begin();
    homerH1.begin();
    // homerM2.begin();
    // homerH2.begin();
    InitSensors1();
    // InitSteppers2();
    // InitSensors2();
    InitEncoder();
    InitButtons();

    //______Motors to zero______
    Homing1();
    // Homing2();
    //homerM1.update();
    //homerH1.update();

    // Move to the right time
    GetAndMoveToTime(true, false);

    while (millis() - lastUpdatedTimeData < adjust_hands_speed)
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

    HandleInputs(); // Check and handle button presses

    switch (state)
    {
    case 0:           // is state is default / display time
        switch (mode) // execute mode functions
        {
        case 0: // Time mode
            if (lastState != state && state == 0)
            {
                GetAndMoveToTime(true, false);
                while (millis() - lastUpdatedTimeData < adjust_hands_speed)
                {
                    delay(1);
                }
            }

            steppersMovingMethod = 0; // time mode
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
        break;

    case 1: // mode change
    {
        if (lastState != state && state == 1)
        {
            reference_encoder_position = encoder.getCount();
            last_encoder_position = reference_encoder_position;
            Serial.printf("Reference encoder position: %d\n", reference_encoder_position);
        }

        int encoderDelta = GetPostionFromEncoder(encoder) - reference_encoder_position;
        selectedMode = encoderDelta; // each two clicks is one mode step
        if (selectedMode < 0)
        {
            selectedMode = 0;
            reference_encoder_position = GetPostionFromEncoder(encoder);
        }
        else if (selectedMode > 2)
        {
            selectedMode = 2;
            reference_encoder_position = GetPostionFromEncoder(encoder) - 2;
        }

        // Set minute and hour hands to zero, second hand to mode position
        clock1TargetPositions[0] = 0;                   // minute hand
        clock1TargetPositions[1] = 0;                   // hour hand
        clock1TargetPositions[2] = (20480 / 12) * selectedMode; // second hand

        steppersMovingMethod = 1; // multi stepper
    }
    break;

    case 2: // settings
        if (lastState == 1 && state == 2)
        {
            mode = selectedMode; // confirm selected mode
            Serial.printf("Mode set to: %d\n", mode);
            Serial.print("entering settings state");
        }

        if (lastState != state && state == 2)
        {
            reference_encoder_position = encoder.getCount();
            last_encoder_position = reference_encoder_position;
            Serial.printf("Reference encoder position: %d\n", reference_encoder_position);
        }

        int encoderDelta = GetPostionFromEncoder(encoder) - reference_encoder_position;
           


        switch (mode)
        {
        case 1: // Time to mode
            const int stepsPerTurn = 20480;
            const double stepsPerHour = (double)stepsPerTurn / 12.0; // 1706.666...
            const double stepsPerMinute = stepsPerHour / 60.0;       // ~28.444...
            const double stepsPerSecond = stepsPerMinute / 60.0;       // ~5.6889

            if (selectedClock == 0)
            {
                switch (selectedHand)
                {
                case 0: // hour hand
                    timeToHour = encoderDelta; // each step is 5 hours
                    break;
                
                case 1: // minute hand
                    timeToMinute = encoderDelta; // each step is 5 minutes
                    break;
                
                case 2: // second hand
                    timeToSecond = encoderDelta; // each step is 5 seconds
                }

                clock1TargetPositions[0] = timeToHour * stepsPerHour;                   // minute hand
                clock1TargetPositions[1] = timeToMinute * stepsPerMinute;                   // hour hand
                clock1TargetPositions[2] = timeToSecond * stepsPerSecond; // second hand

                clock2TargetPositions[0] = 0;                   // minute hand
                clock2TargetPositions[1] = 0;                   // hour hand
                clock2TargetPositions[2] = 0; // second hand
            }
            else
            {
                clock1TargetPositions[0] = 0;                   // minute hand
                clock1TargetPositions[1] = 0;                   // hour hand
                clock1TargetPositions[2] = 0; // second hand

                clock2TargetPositions[0] = timeToHour * stepsPerHour;                   // minute hand
                clock2TargetPositions[1] = timeToMinute * stepsPerMinute;                   // hour hand
                clock2TargetPositions[2] = timeToSecond * stepsPerSecond; // second hand
            }
            break;
        
        }

        steppersMovingMethod = 1; // multi stepper

        break;

    }

    switch (steppersMovingMethod)
    {
    case 0: // Time mode
        if (clock1active)
            SetTimeSpeed(S1, M1, H1);
        RunHandsTime1();

        if (clock2active)
            SetTimeSpeed(S2, M2, H2);
        RunHandsTime2();

        break;
    case 1: // Multi stepper
        if (clock1active)
            SetModeChangeSpeed(S1, M1, H1);
        Clock1.moveTo(clock1TargetPositions);
        Clock1.run();
        if (clock2active)
            SetModeChangeSpeed(S2, M2, H2);
        Clock2.moveTo(clock2TargetPositions);
        Clock2.run();

    case 2: // Individual run speed
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

    // Update step count

    UpdateMotorsStepCount();

    if (state != lastState) {
        calibrateSecondHand(S1, SensorS1);
    }

    // Save last mode and state
    lastMode = mode;
    lastState = state;

    // if (loopTimestamp % 2000 == 10)
    //{
    //     Serial.printf("Mode: %d, State: %d, SteppersMethod: %d\n", mode, state, steppersMovingMethod);
    // }
}