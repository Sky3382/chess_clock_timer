#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include "globals.h"
//#include "steppers.h"
#include "modes.h"
#include "time_utils.h"
#include "wifi_setup.h"
#include "input.h"
#include "clocks.h"

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
    Clock1.begin();
    //Clock2.begin();
    Clock1.setModeChangeSpeed();
    

    InitEncoder();
    InitButtons();

    //______Motors to zero______
    Clock1.home();
    //Clock2.home();

    Clock1.getAndMoveToTime();

    Serial.println("Clock on time");
    lastUpdatedSteps = millis();
    //lastUpdatedHour = millis();
}

void loop()
{
    Clock1.setTimeSpeed();
    Clock1.runTimeMode();
    // // --- Record current loop timestamp ---
    // loopTimestamp = millis();

    // // --- Handle button and encoder inputs ---
    // HandleInputs();

    // // =========================================================
    // // ===============      STATE       =================
    // // =========================================================
    // switch (state)
    // {
    // // ---------------------------------------------------------
    // // STATE 0: Default / Display time on clocks
    // // ---------------------------------------------------------
    // case 0:
    //     switch (mode)
    //     {
    //     // --- MODE 0: Time display mode (normal operation) ---
    //     case 0:
    //     // When entering this mode for the first time
    //     if (lastState != state && state == 0)
    //     {
    //         GetAndMoveToTime(true, false); // Move clock to current time
    //         clock1active = true;
    //         clock2active = false;
    //         steppersMovingMethod = 0; // Use time mode for stepper control
    //         while (millis() - lastUpdatedTimeData < adjust_hands_speed)
    //             delay(1); // Wait until adjustment is complete
    //     }
    //     else
    //     {
    //         // Ensure steppers remain active and target positions are updated
    //         clock1active = true;
    //         clock2active = false;
    //         steppersMovingMethod = 0;
    //     }


    //     break;

    //     // --- MODE 1: "Time To" mode (set a specific target time) ---
    //     case 1:
    //         if (lastState != state && state == 0)
    //         {
    //             // Normalize time values to stay within valid ranges
    //             if (timeToHour < 0) timeToHour += 12;
    //             if (timeToHour >= 12) timeToHour -= 12;

    //             if (timeToMinute < 0) { timeToMinute += 60; timeToHour -= 1; }
    //             if (timeToMinute >= 60) { timeToMinute -= 60; timeToHour += 1; }

    //             if (timeToSecond < 0) { timeToSecond += 60; timeToMinute -= 1; }
    //             if (timeToSecond >= 60) { timeToSecond -= 60; timeToMinute += 1; }

    //             // Move clock hands to the “time-to” position
    //             MoveToTimeTo(timeToClock);

    //             // Wait until movement completes
    //             while (millis() - lastUpdatedTimeData < adjust_hands_speed)
    //                 delay(1);
    //         }

    //         // --- Check if any clock is active (moved away from zero) ---
    //         switch (timeToClock)
    //         {
    //         case 0:
    //             if (S1.currentPosition() == 0 && M1.currentPosition() == 0 && H1.currentPosition() == 0)
    //                 clock1active = false;
    //             else
    //                 clock1active = true;
    //             break;

    //         case 1:
    //             if (S2.currentPosition() == 0 && M2.currentPosition() == 0 && H2.currentPosition() == 0)
    //                 clock1active = false;
    //             else
    //                 clock1active = true;
    //             break;
    //         }

    //         steppersMovingMethod = 0; // Multi-stepper mode
    //         break;

    //     // --- MODE 2: Pomodoro timer mode ---
    //     case 2:
    //         clock1active = false; // Only clock 2 runs in this mode
    //         clock2active = true;
    //         steppersMovingMethod = 1; // Run steppers individually
    //         break;

    //     // --- Any other mode defaults to normal time display ---
    //     default:
    //         clock1active = true;
    //         clock2active = false;
    //         steppersMovingMethod = 0; // Time mode
    //     }
    //     break;

    // // ---------------------------------------------------------
    // // STATE 1: Mode change (using rotary encoder)
    // // ---------------------------------------------------------
    // case 1:
    // {
    //     // When entering mode change state
    //     if (lastState != state && state == 1)
    //     {
    //         reference_encoder_position = GetPostionFromEncoder(encoder);
    //         last_encoder_position = reference_encoder_position;
    //         selectedMode = mode; // Start with the current mode
    //         Serial.printf("Reference encoder position: %d\n", reference_encoder_position);
    //     }

    //     // Calculate encoder movement delta
    //     int encoderDelta = GetPostionFromEncoder(encoder) - reference_encoder_position;
    //     selectedMode = encoderDelta; // Each 2 encoder steps = 1 mode step

    //     // Clamp mode selection between 0 and 2
    //     if (selectedMode < 0)
    //     {
    //         selectedMode = 0;
    //         reference_encoder_position = GetPostionFromEncoder(encoder);
    //     }
    //     else if (selectedMode > 2)
    //     {
    //         selectedMode = 2;
    //         reference_encoder_position = GetPostionFromEncoder(encoder) - 2;
    //     }

    //     // Move second hand to indicate current mode
    //     clock1TargetPositions[0] = 0;                           // Minute hand at zero
    //     clock1TargetPositions[1] = 0;                           // Hour hand at zero
    //     clock1TargetPositions[2] = NormalizeStep((20480 / 12) * selectedMode); // Second hand points to mode

    //     steppersMovingMethod = 1; // Use multi-stepper method
    // }
    // break;

    // // ---------------------------------------------------------
    // // STATE 2: Settings (adjust time or parameters)
    // // ---------------------------------------------------------
    // case 2:
    //     // Entering settings from mode selection
    //     if (lastState == 1)
    //     {
    //         mode = selectedMode; // Confirm selected mode
    //         Serial.printf("Mode set to: %d\n", mode);
    //         Serial.println("Entering settings state...");
    //     }

    //     // Initialize settings mode
    //     if (lastState != state)
    //     {
    //         encoder.setCount(0);
    //         selectedHand = 0; // Start adjusting hour hand
    //         Serial.printf("Reference encoder position: %d\n", reference_encoder_position);
    //         Serial.printf("Selected hand: %d\n", selectedHand);
    //     }

    //     // If selected hand changed, reset encoder reference
    //     if (selectedHand != previousSelectedHand)
    //     {
    //         encoder.setCount(0);
    //         previousSelectedHand = selectedHand;
    //         Serial.printf("Selected hand changed to: %d\n", selectedHand);
    //         Serial.printf("Hands position - Hour: %d, Minute: %d, Second: %d\n",
    //                       timeToHour, timeToMinute, timeToSecond);
    //     }

    //     // --- Encoder movement updates selected hand’s position ---
    //     int encoderDelta = GetPostionFromEncoder(encoder);

    //     switch (mode)
    //     {
    //     // Only applies to Time-To mode
    //     case 1:

    //         // Adjust selected hand value
    //         switch (selectedHand)
    //         {
    //         case 0: timeToHour = lastTimeToHour + encoderDelta; break;
    //         case 1: timeToMinute = lastTimeToMinute + encoderDelta; break;
    //         case 2: timeToSecond = lastTimeToSecond + encoderDelta; break;
    //         }

    //         // Update motor target positions based on selected clock
    //         timeToClock = selectedClock;
    //         if (selectedClock == 0)
    //         {
    //             clock1TargetPositions[0] = NormalizeStep((int)(timeToHour * hourStepsPerHour + timeToMinute * hourStepsPerMinute + timeToSecond * hourStepsPerSecond) % stepsPerTurn);
    //             clock1TargetPositions[1] = NormalizeStep((int)(timeToMinute * minuteStepsPerMinute + timeToSecond * minuteStepsPerSecond) % stepsPerTurn);
    //             clock1TargetPositions[2] = NormalizeStep((int)(timeToSecond * secondStepsPerSecond) % stepsPerTurn);

    //             clock2TargetPositions[0] = 0;
    //             clock2TargetPositions[1] = 0;
    //             clock2TargetPositions[2] = 0;
    //         }
    //         else
    //         {
    //             clock1TargetPositions[0] = 0;
    //             clock1TargetPositions[1] = 0;
    //             clock1TargetPositions[2] = 0;

    //             clock2TargetPositions[0] = NormalizeStep((int)(timeToHour * hourStepsPerHour + timeToMinute * hourStepsPerMinute + timeToSecond * hourStepsPerSecond) % stepsPerTurn);
    //             clock2TargetPositions[1] = NormalizeStep((int)(timeToMinute * minuteStepsPerMinute + timeToSecond * minuteStepsPerSecond) % stepsPerTurn);
    //             clock2TargetPositions[2] = NormalizeStep((int)(timeToSecond * secondStepsPerSecond) % stepsPerTurn);
    //         }
    //         break;
    //     }

    //     steppersMovingMethod = 1; // Use multi-stepper
    //     break;
    // }

    // // =========================================================
    // // ===============   STEPPER CONTROL LOGIC   ===============
    // // =========================================================
    // switch (steppersMovingMethod)
    // {
    // // --- Time mode (normal real-time movement) ---
    // case 0:
    //     if (clock1active)
    //     {
    //         SetTimeSpeed(S1, M1, H1);
    //         RunHandsTime1();
    //     }
    //     if (clock2active)
    //     {
    //         SetTimeSpeed(S2, M2, H2);
    //         RunHandsTime2();
    //     }
    //     break;

    // // --- Multi-stepper mode (synchronized movement to target) ---
    // case 1:
    //     clock1TargetPositions[0] %= stepsPerTurn;
    //     clock1TargetPositions[1] %= stepsPerTurn;
    //     clock1TargetPositions[2] %= stepsPerTurn;

    //     clock2TargetPositions[0] %= stepsPerTurn;
    //     clock2TargetPositions[1] %= stepsPerTurn;
    //     clock2TargetPositions[2] %= stepsPerTurn;
    //     if (clock1active)
    //     {
    //         SetModeChangeSpeed(S1, M1, H1);
    //         Clock1.moveTo(clock1TargetPositions);
    //         Clock1.run();
    //     }

    //     if (clock2active)
    //     {
    //         SetModeChangeSpeed(S2, M2, H2);
    //         Clock2.moveTo(clock2TargetPositions);
    //         Clock2.run();
    //     }
    //     break;

    // // --- Individual run speed mode (each motor runs independently) ---
    // case 2:
    //     if (clock1active)
    //     {
    //         S1.runSpeed();
    //         M1.runSpeed();
    //         H1.runSpeed();
    //     }

    //     if (clock2active)
    //     {
    //         S2.runSpeed();
    //         M2.runSpeed();
    //         H2.runSpeed();
    //     }
    //     break;
    // }

    // // =========================================================
    // // ===============   MAINTENANCE OPERATIONS  ===============
    // // =========================================================
    // UpdateMotorsStepCount(); // Track motor step counts

    // // Calibrate second hand if state changed
    // if (state != lastState)
    //     calibrateSecondHand(S1, SensorS1);

    // // Save current mode/state for next loop iteration
    // lastMode = mode;
    // lastState = state;

    // // Debug logging (disabled by default)
    // // if (loopTimestamp % 2000 == 10)
    // //     Serial.printf("Mode: %d, State: %d, SteppersMethod: %d\n", mode, state, steppersMovingMethod);
}
