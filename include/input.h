#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include <ESP32encoder.h>
#include <Bounce2.h>

// Rotary Encoder Pins
#define ENCODER1_PIN_A 39
#define ENCODER1_PIN_B 40

#define BUTTON_ENCODER_PIN 4
#define BUTTON_BACK_PIN 5
#define BUTTON_NEXT_PIN 6

#define switchPin 12

#define BUTTON_DEBOUNCE_INTERVAL 50 // milliseconds

// Rotary Encoder Instance
extern ESP32Encoder encoder;

// Button Instances
extern Bounce button_encoder;
extern Bounce button_back;
extern Bounce button_next;

extern bool switchState;

extern int reference_encoder_position; // position when entering state 1
extern int last_encoder_position;      // track previous position

extern void InitEncoder();
extern void InitButtons();
extern void HandleInputs();

extern int GetPostionFromEncoder(ESP32Encoder encoderinquestion);

#endif