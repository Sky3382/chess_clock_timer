#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include <ESP32encoder.h>
#include <Bounce2.h>

// Rotary Encoder Pins
#define ENCODER1_PIN_A 2
#define ENCODER1_PIN_B 3

#define BUTTON_ENCODER_PIN 4
#define BUTTON_STATE_PIN 5
#define BUTTON_MODE_PIN 6
#define BUTTON_ENTER_PIN 7

#define BUTTON_DEBOUNCE_INTERVAL 50 // milliseconds

// Rotary Encoder Instance
extern ESP32Encoder encoder;

// Button Instances
extern Bounce button_encoder;
extern Bounce button_state;
extern Bounce button_mode;
extern Bounce button_enter;

extern void InitEncoder();
extern void InitButtons();

#endif