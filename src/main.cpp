#include "main.h"

void setup() {
  Serial.begin(115200);

  while (!Serial)
    ;

  Serial.println(F("OBD2 Supported PIDs"));

  while (true) {
    Serial.print(F("Attempting to connect to OBD2 CAN bus ... "));

    if (!OBD2.begin()) {
      Serial.println(F("failed!"));

      delay(1000);
    } else {
      Serial.println(F("success"));
      break;
    }
  }

  Serial.println();

  // loop through PIDs 0 to 95, reading and printing the names of the supported PIDs
  for (int pid = 0; pid < 96; pid++) {
    if (OBD2.pidSupported(pid)) {
      Serial.println(OBD2.pidName(pid));
    }
  }

  while (!OBD2.begin())
    delay(200);

  FastLED.addLeds<WS2813, LED_PIN, RGB>(leds, NUM_LEDS, 0);
  FastLED.setBrightness(LED_MAX_BRIGHTNESS);

  //   Serial.println("Setup is DONE !!!");
}

void loop() {
  ledsLoop();

  int rpm = OBD2.pidRead(ENGINE_RPM);

  // Check Engine State
  if (rpm < TURN_OFF_RPM) {
    if (stateOfDevices != offAll) {
      // Serial.println("Engine is just turned off and display and led are on");
      delay(1000);
      switchState(offAll);
      // introPresented = false;
      OBD2.end();  // ================================
                   // Check what happens if end connection and read rpm
    } else {
      // Delay programm if engine is off
      // Serial.println("Engine is off and leds are off");
      delay(1000);
    }
  } else if (rpm > TURN_OFF_RPM && stateOfDevices == offAll && !introPresented) {
    // Serial.println("Engine is on");

    // switchState(onAll);
    // delay(1000);
    // intro();
    // introPresented = true;
    // switchState(offAll);

    // while (!OBD2.begin())
    // delay(200);
  }
}

void ledsLoop() {
  int rpm = OBD2.pidRead(ENGINE_RPM);

  if (!isnan(rpm)) {
    int level = map(rpm, RPM_MIN, RPM_MAX, 0, NUM_LEDS);
    level = constrain(level, 0, NUM_LEDS);

    fill_gradient_RGB(leds, NUM_LEDS, CRGB{255, 255, 0}, CRGB{255, 0, 0});

    for (int i = level; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;

    // Blink
    if (rpm >= BLINK_RPM) {
      if (!isBlinkingRPMLimitPassed) {
        isBlinkingRPMLimitPassed = true;
        ledBlinkStartMillis = millis();
      }

      if (millis() - ledBlinkStartMillis >= BLINK_DURATION) {
        colorsAreTurnedOn = !colorsAreTurnedOn;
        ledBlinkStartMillis = millis();
      }

      if (!colorsAreTurnedOn)
        FastLED.clear();

    } else {
      isBlinkingRPMLimitPassed = false;
    }

    FastLED.show();
  }
}

// void printDataToScreen() {
//   lcd.home();

//   switch (preset) {
//     default:
//       printValue("Injection: ", FUEL_RAIL_GAUGE_PRESSURE, 0, 0);
//       printValue("Intake:       ", INTAKE_MANIFOLD_ABSOLUTE_PRESSURE, 0, 1);
//       printTemp("Intake Temp:  ", AIR_INTAKE_TEMPERATURE, 0, 2);
//       printTemp("Coolant Temp: ", ENGINE_COOLANT_TEMPERATURE, 0, 3);
//       break;

//     case 1:
//       printTemp("Outside Temp: ", AMBIENT_AIR_TEMPERATURE, 0, 0);
//       printTemp("Intake Temp:  ", AIR_INTAKE_TEMPERATURE, 0, 1);
//       printTemp("Catalyst 1:   ", CATALYST_TEMPERATURE_BANK_1_SENSOR_1, 0, 2);
//       printTemp("Catalyst 2:   ", CATALYST_TEMPERATURE_BANK_2_SENSOR_1, 0, 3);
//       break;

//     case 2:
//       printValue("RPM:    ", ENGINE_RPM, 0, 0);
//       printValue("Speed:  ", VEHICLE_SPEED, 0, 1);
//       printValue("Load:   ", CALCULATED_ENGINE_LOAD, 0, 2);
//       printValue("Fuel:   ", FUEL_TANK_LEVEL_INPUT, 0, 3);
//       break;
//   }
// }

// void printValue(String title, int pid, int column, int row) {
//   float value = OBD2.pidRead(pid);
//   String units = OBD2.pidUnits(pid);

//   if (!isnan(value)) {
//     lcd.setCursor(column, row);

//     lcd.print(title);

//     // Clear value spaces
//     for (unsigned int i = 0; i < (20 - strlen(title.c_str())); i++)
//       lcd.print(" ");

//     lcd.setCursor(column + strlen(title.c_str()), row);

//     if (roundf(value) == value)
//       lcd.print(int(value));
//     else
//       lcd.print(value);

//     lcd.print(" ");
//     lcd.print(units);
//   }
// }
