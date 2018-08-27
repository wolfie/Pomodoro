#include <InternetButton.h>
#include <math.h>
#include "VirtualLeds.h"

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

#define RESET_HOLD_TIME 500.0
#define BLINK_INTERVAL 5000.0

#define CLICKS_FOR_ONLINE 3
#define MAX_DELAY_BETWEEN_CLICKS_FOR_ONLINE 500.0

#define STATE_WAIT 0
#define STATE_COUNTING 1
#define STATE_REST 2
#define STATE_PAUSE 3
#define STATE_ONLINE 4

InternetButton b = InternetButton();
VirtualLeds leds = VirtualLeds(b);
VirtualLeds pauseLeds = VirtualLeds(b);

float endMillis;
float resetHoldEnd;
bool wasStartPausePressed;
bool resetSoundWasPlayed;
uint8_t state;
uint8_t iterationsDone;

float pauseStart;
uint8_t stateBeforePause;

bool wasResetPressed;
uint8_t onlineClicks;
float lastOnlineClickTime;

SYSTEM_MODE(MANUAL);

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

  wasResetPressed = 0;
  onlineClicks = 0;
  lastOnlineClickTime = 0.0;
}

void loop() {
  float now = millis();

  if (b.buttonOn(3)) wasStartPausePressed = true;
  if (b.buttonOn(1)) {
    wasResetPressed = true;
    handleHoldToReset(now);
  } else {
    resetHoldEnd = 0;
    resetSoundWasPlayed = false;
  }

  switch (state) {
    case STATE_WAIT: wait(now); break;
    case STATE_COUNTING: counting(now); break;
    case STATE_REST: rest(now); break;
    case STATE_PAUSE: pause(now); break;
    case STATE_ONLINE: online(now); break;
    default: break;
  }

  if (!b.buttonOn(3)) wasStartPausePressed = false;
  if (!b.buttonOn(1)) {
    if (wasResetPressed) {
      handleMultipleClicksForOnlineMode(now);
    }
    wasResetPressed = false;
  }

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
  leds.allLedsOn(colorProgress, colorProgress, colorProgress);
  showCheckmarks();
}

void pause(float now) {
  if (fmod(now, BLINK_INTERVAL) < BLINK_INTERVAL/2) {
    leds.copyFrom(pauseLeds);
  } else {
    leds.allLedsOff();
  }

  if (wasStartPauseReleased()) {
    b.playNote("C5", 8);
    endMillis += (now - pauseStart);
    state = stateBeforePause;
    leds.copyFrom(pauseLeds);
  }
}

void online(float now) {
  if (Particle.connected() == false) {
    // We need to write to leds directly, since the connection call is synchronous.
    b.allLedsOn(128, 0, 128);
    Particle.connect();
    waitFor(Particle.connected, 5000);
  } else {
    if (fmod(now, BLINK_INTERVAL) < BLINK_INTERVAL/2) {
      leds.allLedsOn(128, 128, 0);
    } else {
      leds.allLedsOn(64, 64, 0);
    }
    Particle.process();
    delay(200);
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
  pauseLeds.copyFrom(leds);
  stateBeforePause = state;
  state = STATE_PAUSE;
  pauseStart = now;
  b.playNote("C5", 8);
}

void handleHoldToReset(float now) {
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
}

void handleMultipleClicksForOnlineMode(float now) {
  const float delta = now - lastOnlineClickTime;
  if (delta > MAX_DELAY_BETWEEN_CLICKS_FOR_ONLINE) {
    onlineClicks = 1;
  }

  if (onlineClicks >= CLICKS_FOR_ONLINE) {
    state = STATE_ONLINE;
    onlineClicks = 0;
  }

  onlineClicks++;
  lastOnlineClickTime = now;
}
