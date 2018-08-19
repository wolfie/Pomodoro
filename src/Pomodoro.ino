#include <InternetButton.h>
#include <math.h>
#include "VLeds.h"

#define DEVMODE false

#if DEVMODE
  #define POMODORO_TIME 15000L
  #define NORMAL_REST_TIME 3000L
  #define LONG_REST_TIME 18000L
#else
  #define POMODORO_TIME 1500000L // 25mins
  #define NORMAL_REST_TIME 300000L // 5mins
  #define LONG_REST_TIME 1800000L // 30mins
#endif

#define RESET_HOLD_TIME 500L

#define STATE_WAIT 0
#define STATE_COUNTING 1
#define STATE_REST 2
#define STATE_PAUSE 3

InternetButton b = InternetButton();
VLeds leds = VLeds(b);

float endMillis;
float resetHoldEnd;
bool wasStartPausePressed;
bool resetSoundWasPlayed;
uint8_t state;
uint8_t iterationsDone;

float pauseStart;
uint8_t stateBeforePause;

void setup() {
  b.begin();

  iterationsDone = 0;
  state = STATE_WAIT;
  wasStartPausePressed = false;
  endMillis = 0;
  resetHoldEnd = 0;
  resetSoundWasPlayed = false;

  pauseStart = 0;
  stateBeforePause = 0;
}

void loop() {
  float now = millis();

  if (b.buttonOn(3)) wasStartPausePressed = true;
  if (b.buttonOn(1)) {
    if (resetHoldEnd == 0) resetHoldEnd = now + RESET_HOLD_TIME;
    else if (now > resetHoldEnd) {
      state = STATE_WAIT;
      iterationsDone = 0;
      endMillis = 0;
      leds.allLedsOff();
      if (!resetSoundWasPlayed) {
        b.playNote("C5", 8);
        resetSoundWasPlayed = true;
      }
    }
  } else {
    resetHoldEnd = 0;
    resetSoundWasPlayed = false;
  }

  switch (state) {
    case STATE_WAIT: wait(now); break;
    case STATE_COUNTING: counting(now); break;
    case STATE_REST: rest(now); break;
    case STATE_PAUSE: pause(now); break;
    default: break;
  }

  if (!b.buttonOn(3)) wasStartPausePressed = false;

  leds.loop();
}

void wait(float now) {
  showCheckmarks();
  if (wasStartPauseReleased()) {
    state = STATE_COUNTING;
    iterationsDone %= 4;
    b.playNote("C6", 16);
  }
}

void counting(float now) {
  if (endMillis < 1) {
    endMillis = now + POMODORO_TIME;
  }

  if (wasStartPauseReleased()) {
    initPause(now);
  }

  const float progress = (endMillis - now) / POMODORO_TIME;
  if (progress < 0) {
    state = STATE_REST;
    endMillis = 0;
    iterationsDone++;
    b.playNote("C5", 16);
    b.playNote("R", 16);
    b.playNote("C5", 8);
    return;
  }

  const float ledProgress = progress * 11;
  for (uint8_t led = 1; led <= floor(ledProgress); led++) {
    leds.ledOn(led, 255, 0, 0);
  }

  const uint8_t lastLed = (uint8_t) ceil(ledProgress);
  const float lastLedProgress = ledProgress - floor(ledProgress);
  const uint8_t lastLedRedness = (uint8_t)round(lastLedProgress * 255);
  leds.ledOn(lastLed, lastLedRedness, 0, 0);

  for (uint8_t led = lastLed+1; led <= 11; led++) {
    leds.ledOff(led);
  }
}

void rest(float now) {
  const float restTime = iterationsDone < 4 ? NORMAL_REST_TIME : LONG_REST_TIME;
  if (endMillis < 1) endMillis = now + restTime;

  if (wasStartPauseReleased()) {
    initPause(now);
  }

  const float progress = (endMillis - now) / restTime;
  if (progress < 0) {
    state = STATE_WAIT;
    endMillis = 0;

    if (iterationsDone < 4) {
      b.playNote("C5", 16);
      b.playNote("C6", 8);
    } else {
      b.playNote("C5", 16);
      b.playNote("C6", 16);
      b.playNote("C5", 8);
    }
    return;
  }

  const float colorProgress = progress * 255;
  for (uint8_t led = 1; led <= 11; led++) {
    leds.ledOn(led, colorProgress, colorProgress, colorProgress);
  }
  showCheckmarks();
}

void pause(float now) {
  if (wasStartPauseReleased()) {
    b.playNote("C5", 8);
    endMillis += (now - pauseStart);
    state = stateBeforePause;
  }
}

void showCheckmarks() {
  for (uint8_t i = iterationsDone; i > 0; i--) {
    if (i == 1) leds.ledOn(10, 0, 255, 0);
    if (i == 2) leds.ledOn(11, 0, 255, 0);
    if (i == 3) leds.ledOn(1, 0, 255, 0);
    if (i == 4) leds.ledOn(2, 0, 255, 0);
  }
}

bool wasStartPauseReleased() {
  return !b.buttonOn(3) && wasStartPausePressed;
}

void initPause(float now) {
  stateBeforePause = state;
  state = STATE_PAUSE;
  pauseStart = now;
  b.playNote("C5", 8);
}
