#include <Arduino.h>

#include <CAN.h>
#include <FastLED.h>
#include <SPI.h>
#include <Wire.h>

// Comment next line and save to change the Environment
#define isTesting

// LED
#ifdef isTesting
// --------------------------- Test Environment ---------------------------------------
#define NUM_LEDS 9              // Number of LEDS to use
#define LED_MAX_BRIGHTNESS 150  // 0 - 255
#define BLINK_DURATION 75       // milliseconds

#define RPM_MIN 900               // Start rpm value
#define RPM_MAX 2000              // End rpm value
#define BLINK_RPM_THRESHOLD 1500  // RPM threshold for blink to start

#else
// --------------------------- Real Environment ---------------------------------------
#define NUM_LEDS 9              // Number of LEDS to use
#define LED_MAX_BRIGHTNESS 150  // 0 - 255
#define BLINK_DURATION 75       // milliseconds

#define RPM_MIN 3000              // Start rpm value
#define RPM_MAX 5000              // End rpm value
#define BLINK_RPM_THRESHOLD 5200  // RPM threshold for blink to start

#endif

// Pins
#define LED_PIN 3

// Threshold at which arduino considers engine turned off
#define TURN_OFF_RPM 200

// Objects
CRGB leds[NUM_LEDS];

bool colorsAreTurnedOn = true;
unsigned long lastTimeLEDsWereToggled = 0;
bool isBlinkingRPMLimitPassed = false;

void ledsLoop();