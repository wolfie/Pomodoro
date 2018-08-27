#include "VirtualLeds.h"
#include <InternetButton.h>

VirtualLeds::VirtualLeds(const InternetButton& button) {
  b = button;
}

void VirtualLeds::loop() {
  for (uint8_t led = 0; led < 11; led++) {
    b.ledOn(led+1, vLeds[led][0], vLeds[led][1], vLeds[led][2]);
  }
}

void VirtualLeds::ledOn(uint8_t led, uint8_t r, uint8_t g, uint8_t b) {
  vLeds[led-1][0] = r;
  vLeds[led-1][1] = g;
  vLeds[led-1][2] = b;
}

void VirtualLeds::ledOff(uint8_t led) {
  ledOn(led, 0,0,0);
}

void VirtualLeds::allLedsOff() {
  allLedsOn(0, 0, 0);
}

void VirtualLeds::allLedsOn(uint8_t r, uint8_t g, uint8_t b) {
  for (uint8_t led = 1; led <= 11; led++) {
    ledOn(led, r, g, b);
  }
}

void VirtualLeds::copyFrom(const VirtualLeds& other) {
  for (uint8_t led = 0; led < 11; led++) {
    vLeds[led][0] = other.vLeds[led][0];
    vLeds[led][1] = other.vLeds[led][1];
    vLeds[led][2] = other.vLeds[led][2];
  }
}
