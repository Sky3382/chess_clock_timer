#include <input.h>
#include <globals.h>
#include <ESP32encoder.h>

// Rotary Encoder Instance
ESP32Encoder encoder;

// Button Instances
Bounce button_encoder;
Bounce button_state;
Bounce button_mode;
Bounce button_enter;

void InitEncoder()
{
    // Initialize encoder
    encoder.attachHalfQuad(ENCODER1_PIN_A, ENCODER1_PIN_B);
    encoder.setCount(0);                       // Set initial count to 0
    pinMode(BUTTON_ENCODER_PIN, INPUT_PULLUP); // Button with pull-up resistor
}

void InitButtons()
{
    pinMode(BUTTON_STATE_PIN, INPUT_PULLUP);   // State button with pull-up resistor
    pinMode(BUTTON_MODE_PIN, INPUT_PULLUP);    // Mode button with pull-up resistor
    pinMode(BUTTON_ENTER_PIN, INPUT_PULLUP);   // Enter button with pull-up resistor
    pinMode(BUTTON_ENCODER_PIN, INPUT_PULLUP); // Encoder button with pull-up resistor

    button_state.attach(BUTTON_STATE_PIN);
    button_state.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval

    button_mode.attach(BUTTON_MODE_PIN);
    button_mode.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval

    button_enter.attach(BUTTON_ENTER_PIN);
    button_enter.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval

    button_encoder.attach(BUTTON_ENCODER_PIN);
    button_encoder.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval
}

void HandleInputs()
{
    button_state.update();
    button_mode.update();
    button_enter.update();
    button_encoder.update();

    if (button_state.fell())
    {
        if (state == 0)
        {
            state = 1; // Change to mode change state
            Serial.println("Entering mode change state");
        }
        else
        {
            state = 0; // Return to normal mode
            Serial.println("Returning to normal mode");
        }
    }
    if (button_mode.fell())
    {
        Serial.println("Mode button pressed");
        // Handle mode button press
    }
    if (button_enter.fell())
    {
        Serial.println("Enter button pressed");
        // Handle enter button press
    }
    if (button_encoder.fell())
    {
        Serial.println("Encoder button pressed");
        // Handle encoder button press
    }
}
