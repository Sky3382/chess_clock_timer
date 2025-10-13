#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include <ESP32encoder.h>
#include <Adafruit_MCP23X17.h>
#include <Bounce2.h>
#include <AS5600.h>

// MCP Pins
#define MCP_SDA 17
#define MCP_SCL 18

// buttons, switch and encoder (on gpio extender)
#define ENCODER_PIN_A 0
#define ENCODER_PIN_B 1

#define BUTTON_ENCODER_PIN 2
#define BUTTON_BACK_PIN 3
#define BUTTON_NEXT_PIN 4

#define SWITCH_PIN 5

// debounce interval for buttons
#define BUTTON_DEBOUNCE_INTERVAL 50 // milliseconds

// input modules instances
// GPIO extender instance
extern Adafruit_MCP23X17 mcp;

// Rotary Encoder Instance
extern ESP32Encoder encoder;

// Button Instances
extern Bounce button_encoder;
extern Bounce button_back;
extern Bounce button_next;

// Input variables
extern bool isSwitchOn;

extern int reference_encoder_position; // position when entering state 1
extern int encoder_position;
extern int last_encoder_position; // track previous position

// input functions
extern void initGPIOExtender();
extern void InitEncoder();
extern void InitButtons();
extern void HandleInputs();

extern void StoreLastTimeToTimeValues();
extern void StoreLastPomodoroTimeValues();

extern void CycleSelectedHand();

extern int GetPostionFromEncoder(ESP32Encoder encoder_in_question);

// OLD

#endif