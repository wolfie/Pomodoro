#include "VLeds.h"
#include <InternetButton.h>

VLeds::VLeds(const InternetButton& button) {
  b = button;
}

void VLeds::loop() {
  for (uint8_t led = 0; led < 11; led++) {
    b.ledOn(led+1, vLeds[led][0], vLeds[led][1], vLeds[led][2]);
  }
}

void VLeds::ledOn(uint8_t led, uint8_t r, uint8_t g, uint8_t b) {
  vLeds[led-1][0] = r;
  vLeds[led-1][1] = g;
  vLeds[led-1][2] = b;
}

void VLeds::ledOff(uint8_t led) {
  ledOn(led, 0,0,0);
}

void VLeds::allLedsOff() {
  for (uint8_t led = 1; led <= 11; led++) {
    ledOn(led, 0,0,0);
  }
}
