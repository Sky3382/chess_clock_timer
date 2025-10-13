#include <input.h>
#include <globals.h>
#include <ESP32encoder.h>
#include <Adafruit_MCP23X17.h>
#include <modes.h>

// input instances
//  Rotary Encoder Instance
ESP32Encoder encoder;

// Button Instances
Bounce button_encoder;
Bounce button_back;
Bounce button_next;

// input variables
bool isSwitchOn;

int reference_encoder_position = 0; // position when entering state 1
int encoder_position = 0;
int last_encoder_position = 0; // track previous position

void InitEncoder()
{
    // Initialize encoder
    encoder.attachHalfQuad(ENCODER_PIN_A, ENCODER_PIN_B);
    encoder.setCount(0);                       // Set initial count to 0
    pinMode(BUTTON_ENCODER_PIN, INPUT_PULLUP); // Button with pull-up resistor
}

void InitButtons()
{
    pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);    // State button with pull-up resistor
    pinMode(BUTTON_NEXT_PIN, INPUT_PULLUP);    // Mode button with pull-up resistor
    pinMode(BUTTON_ENCODER_PIN, INPUT_PULLUP); // Encoder button with pull-up resistor

    pinMode(SWITCH_PIN, INPUT_PULLUP); // Extra switch with pull-up resistor

    button_back.attach(BUTTON_BACK_PIN);
    button_back.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval

    button_next.attach(BUTTON_NEXT_PIN);
    button_next.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval

    button_encoder.attach(BUTTON_ENCODER_PIN);
    button_encoder.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval
}

void HandleInputs()
{
    // UPDATE BUTTONS
    button_back.update();
    button_next.update();
    button_encoder.update();

    // handle buttons
    if (button_back.fell())
    {
        Serial.println("Back button pressed");
        // handle depending on state
        switch (state)
        {
        case 0:
            break;

        case 1:
            state = 0; // if state is mode change, return to display state
            Serial.println("Back to Display state");

        case 2:
            state = 1; // if in settings, return to mode change state
            Serial.println("Back to Mode Change state");

        default:
            break;
        }
    }

    if (button_next.fell())
    {
        Serial.println("Next button pressed");
        // handle depending on state
        switch (state)
        {
        case 2:

            switch (mode) // in settings state, act different depending on the mode
            {
            case 1:
                StoreLastTimeToTimeValues();
                CycleSelectedHand();
                break;

            case 2:
                StoreLastPomodoroTimeValues();
                CycleSelectedHand();

                break;

            default:
                break;
            }

            break;

        default:
            break;
        }
    }

    if (button_encoder.fell())
    {
        switch (state)
        {
        case 0:
            state = 1; // Change to mode change state
            Serial.println("Entering mode change state");
            break;

        case 1:
            if (selected_mode == 0)
            {
                state = 0; // If in time mode and no change, go back to normal state
                mode = 0;
                Serial.println("Entering normal state in time mode");
            }
            else
            {
                state = 2;            // Change to settings state
                mode = selected_mode; // confirm selected mode
                Serial.println("Entering settings state");
            }

        case 2:
            state = 0; // Change to normal state

            break;
        }
    }

    // UPDATE SWITCH
    isSwitchOn = digitalRead(isSwitchOn) == LOW; // LOW means switch closed

    // handle switch
    if (isSwitchOn)
    {
        selected_clock = 2;
    }
    else
    {
        selected_clock = 1;
    }

    // UPDATE ENCODER POSITION
    if (encoder.getCount() % 2 == 0)
    {                                               // only read every second step to avoid noise
        encoder_position = -encoder.getCount() / 2; // Get current position
        if (encoder_position != last_encoder_position)
        {
            Serial.print("Encoder position: ");
            Serial.println(encoder_position);
            last_encoder_position = encoder_position;
        }
    }
}

void StoreLastTimeToTimeValues()
{
    last_timeTo_hour = timeTo_hour;
    last_timeTo_minute = timeTo_minute;
    last_timeTo_second = timeTo_second;
}

void StoreLastPomodoroTimeValues()
{
    last_pomodoro_timer1_hour = pomodoro_timer1_hour;
    last_pomodoro_timer1_minute = pomodoro_timer1_minute;
    last_pomodoro_timer1_second = pomodoro_timer1_second;

    last_pomodoro_timer2_hour = pomodoro_timer2_hour;
    last_pomodoro_timer2_minute = pomodoro_timer2_minute;
    last_pomodoro_timer2_second = pomodoro_timer2_second;
}

void CycleSelectedHand()
{
    if (selected_clock == 1)
    {
        switch (selected_Clock1_hand)
        {
        case 0:
            selected_Clock1_hand = 1;
            Serial.println("Adjusting Clock1 minute hand");
            break;

        case 1:
            selected_Clock1_hand = 2;
            Serial.println("Adjusting Clock1 second hand");
            break;

        case 2:
            selected_Clock1_hand = 0;
            Serial1.println("Adjusting Clock1 hour hand");
            break;

        default:
            Serial.println("Invalid Clock1 hand selection — resetting to hour hand.");
            selected_Clock1_hand = 0;
            break;
        }
    }
    else if (selected_clock == 2)
    {
        switch (selected_Clock2_hand)
        {
        case 0:
            selected_Clock2_hand = 1;
            Serial.println("Adjusting Clock2 minute hand");
            break;

        case 1:
            selected_Clock2_hand = 2;
            Serial.println("Adjusting Clock2 second hand");
            break;

        case 2:
            selected_Clock2_hand = 0;
            Serial1.println("Adjusting Clock2 hour hand");
            break;

        default:
            Serial.println("Invalid Clock2 hand selection — resetting to hour hand.");
            selected_Clock2_hand = 0;
            break;
        }
    }
    else
    {
        Serial.println("invalid Clock selected");
    }
}

int GetPostionFromEncoder(ESP32Encoder encoder_in_question)
{
    return encoder_in_question.getCount() / 2;
}
