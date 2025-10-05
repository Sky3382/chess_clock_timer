#include <input.h>
#include <globals.h>
#include <ESP32encoder.h>

// Rotary Encoder Instance
ESP32Encoder encoder;

// Button Instances
Bounce button_encoder;
Bounce button_back;
Bounce button_mode;
Bounce button_enter;

int reference_encoder_position = 0; // position when entering state 1
int last_encoder_position = 0;      // track previous position

void InitEncoder()
{
    // Initialize encoder
    encoder.attachHalfQuad(ENCODER1_PIN_A, ENCODER1_PIN_B);
    encoder.setCount(0);                       // Set initial count to 0
    pinMode(BUTTON_ENCODER_PIN, INPUT_PULLUP); // Button with pull-up resistor
}

void InitButtons()
{
    pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);    // State button with pull-up resistor
    pinMode(BUTTON_MODE_PIN, INPUT_PULLUP);    // Mode button with pull-up resistor
    pinMode(BUTTON_ENTER_PIN, INPUT_PULLUP);   // Enter button with pull-up resistor
    pinMode(BUTTON_ENCODER_PIN, INPUT_PULLUP); // Encoder button with pull-up resistor

    button_back.attach(BUTTON_BACK_PIN);
    button_back.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval

    button_mode.attach(BUTTON_MODE_PIN);
    button_mode.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval

    button_enter.attach(BUTTON_ENTER_PIN);
    button_enter.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval

    button_encoder.attach(BUTTON_ENCODER_PIN);
    button_encoder.interval(BUTTON_DEBOUNCE_INTERVAL); // Debounce interval
}

void HandleInputs()
{
    button_back.update();
    button_mode.update();
    button_enter.update();
    button_encoder.update();

    if (button_back.fell())
    {
        if (state == 1)
        {
            state = 0; // Change to mode change state
            Serial.println("Entering mode change state");
        } else if (state == 2)
        {
            state = 1; // Change to settings state
            Serial.println("Entering settings state");
        }
        Serial.println("Back button pressed");
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
        if (state == 0)
        {
            state = 1; // Change to mode change state
            Serial.println("Entering mode change state");
        }
        else if (state == 1)
        {   
            if (selectedMode != mode) { // Only enter settings if mode has changed or not in time mode
                state = 2; // Change to settings state
            Serial.println("Entering settings state");
            } else if (selectedMode == 0) {
                state = 0; // If in time mode and no change, go back to normal state
                mode = 0;
                Serial.println("Entering normal state in time mode");

            } else {
                state = 0; // Change to normal state
                Serial.println("Entering normal state");
            }
            
        } else if (state == 2)
        {
            state = 0; // Change to normal state
            Serial.println("Entering normal state");
        }
    }

    long position;
    if (encoder.getCount() % 2 == 0)
    {                                       // only read every second step to avoid noise
        position = -encoder.getCount() / 2; // Get current position
        if (position != last_encoder_position)
        {
            Serial.print("Encoder position: ");
            Serial.println(position);
            last_encoder_position = position;
        }
    }
}

int GetPostionFromEncoder(ESP32Encoder encoderinquestion)
{
    return -encoderinquestion.getCount() / 2;
}
