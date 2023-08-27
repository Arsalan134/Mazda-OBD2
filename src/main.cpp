#include "main.h"
// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.
//
//
// This examples queries the engine RPM (OBD-II PID 0x0c) once a seconds and
// prints the value to the serial monitor
//

// Most cars support 11-bit adddress, others (like Honda),
// require 29-bit (extended) addressing, set the next line
// to true to use extended addressing
const bool useStandardAddressing = true;

void setup() {
  delay(1000);

  Serial.begin(9600);

  delay(1000);

  Serial.println("CAN OBD-II engine RPM");

  // start the CAN bus at 500 kbps
  while (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    delay(500);
  }

  Serial.println("Starting CAN SUCCESS!");

  // add filter to only receive the CAN bus ID's we care about
  if (useStandardAddressing) {
    CAN.filter(0x7e8);
  } else {
    CAN.filterExtended(0x18daf110);
  }

  // Setup LED library
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS, 0);
  FastLED.setBrightness(LED_MAX_BRIGHTNESS);
}

void loop() {
  if (useStandardAddressing) {
    CAN.beginPacket(0x7df, 8);
  } else {
    CAN.beginExtendedPacket(0x18db33f1, 8);
  }

  CAN.write(0x02);  // number of additional bytes
  CAN.write(0x01);  // show current data
  CAN.write(0x0c);  // engine RPM
  CAN.endPacket();

  // wait for response
  while (CAN.parsePacket() == 0 || CAN.read() < 3 ||  // correct length
         CAN.read() != 0x41 ||                        // correct mode
         CAN.read() != 0x0c)
    ;  // correct PID

  float rpm = ((CAN.read() * 256.0) + CAN.read()) / 4.0;

  Serial.print("Engine RPM = ");
  Serial.println(rpm);

  // wait for response
  // while (!CAN.parsePacket()) {
  //   int response = CAN.read();

  //   // correct length; correct mode;  correct PID
  //   while (response < 3 || response != 0x41 || response != 0x0c)
  //     ;
  // }

  // int rpm = ((CAN.read() * 256.0) + CAN.read()) / 4.0;

  // Check Engine State
  // if (rpm < TURN_OFF_RPM) {
  //   // Serial.println("Engine is just turned off and display and led are on");

  //   CAN.end();  // ================================
  //               // Check what happens if end connection and read rpm
  //   // Delay programm if engine is off
  //   // Serial.println("Engine is off and leds are off");

  //   delay(1000);

  //   return;
  // }

  ledsLoop();
}

void ledsLoop() {
  // Get rpm from OBD

  float rpm = ((CAN.read() * 256.0) + CAN.read()) / 4.0;

  // Cursor that shows current rpm value among leds
  int level = map(rpm, RPM_MIN, RPM_MAX, 0, NUM_LEDS);
  level = constrain(level, 0, NUM_LEDS);

  // Prefill LED Strip with colors
  fill_gradient_RGB(leds, NUM_LEDS, CRGB{255, 255, 0}, CRGB{255, 0, 0});

  // Uncolor leds that are beyond the cursor
  for (int i = level; i < NUM_LEDS; i++)
    leds[i] = CRGB::Black;

  // Blink part
  // if (rpm >= BLINK_RPM_THRESHOLD) {
  //   if (!isBlinkingRPMLimitPassed) {
  //     isBlinkingRPMLimitPassed = true;
  //     lastTimeLEDsWereToggled = millis();
  //   }

  //   // Checking how long leds are on.
  //   // If they are on or off too long, we will toggle them
  //   if (millis() - lastTimeLEDsWereToggled >= BLINK_DURATION) {
  //     colorsAreTurnedOn = !colorsAreTurnedOn;
  //     lastTimeLEDsWereToggled = millis();
  //   }

  //   if (!colorsAreTurnedOn)
  //     // Turn off led
  //     FastLED.clear();

  // } else
  //   // We are out of rpm blinking defined range limits
  //   isBlinkingRPMLimitPassed = false;

  // Send data to a led strip
  FastLED.show();
}