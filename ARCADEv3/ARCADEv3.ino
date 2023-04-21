/*
 * This code programs a number of pins on an ESP32 as buttons on a BLE gamepad
 * It uses arrays to cut down on code
 * Uses the Bounce2 library to debounce all buttons
 * Uses the rose/fell states of the Bounce instance to track button states
 * Before using, adjust the numOfButtons, buttonPins and physicalButtons to suit your senario
 */

#define BOUNCE_WITH_PROMPT_DETECTION  // Make button state changes available immediately

#include <Arduino.h>
#include <Bounce2.h>     // https://github.com/thomasfredericks/Bounce2
#include <BleGamepad.h>  // https://github.com/lemmingDev/ESP32-BLE-Gamepad

#define numOfButtons 13

Bounce debouncers[numOfButtons];

//BleGamepad bleGamepad("Name1", "Name1", 100);
BleGamepad bleGamepad("Name2", "Name2", 100);

byte buttonPins[numOfButtons] = { 4, 14, 16, 17, 19, 21, 22, 23, 25, 26, 27, 32, 33 };
byte physicalButtons[numOfButtons] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

void setup() {
  for (byte currentPinIndex = 0; currentPinIndex < numOfButtons; currentPinIndex++) {
    pinMode(buttonPins[currentPinIndex], INPUT_PULLUP);
    debouncers[currentPinIndex] = Bounce();
    debouncers[currentPinIndex].attach(buttonPins[currentPinIndex]);  // After setting up the button, setup the Bounce instance :
    debouncers[currentPinIndex].interval(5);
  }

  BleGamepadConfiguration bleGamepadConfig;
  bleGamepadConfig.setButtonCount(numOfButtons);
  bleGamepadConfig.setAutoReport(false);
  bleGamepad.begin(&bleGamepadConfig);
}

void loop() {
  if (bleGamepad.isConnected()) {
    bool sendReport = false;
    for (byte currentIndex = 0; currentIndex < numOfButtons; currentIndex++) {
      debouncers[currentIndex].update();

      if (debouncers[currentIndex].fell()) {
        bleGamepad.press(physicalButtons[currentIndex]);
        sendReport = true;
      } else if (debouncers[currentIndex].rose()) {
        bleGamepad.release(physicalButtons[currentIndex]);
        sendReport = true;
      }
    }
    if (sendReport) {
      bleGamepad.sendReport();
    }
  }
}
