#include <InternetButton.h>
#include <math.h>

/*
#define POMODORO_TIME 1500000L // 25mins
#define NORMAL_REST_TIME 300000L // 5mins
#define LONG_REST_TIME 1800000L // 30mins
*/

#define POMODORO_TIME 15000L
#define NORMAL_REST_TIME 3000L
#define LONG_REST_TIME 18000L

#define RESET_HOLD_TIME 500L

#define STATE_WAIT 0
#define STATE_COUNTING 1
#define STATE_REST 2

InternetButton b = InternetButton();

float endMillis;
float resetHoldEnd;
bool wasStartPausePressed;
bool resetSoundWasPlayed;
uint8_t state;
uint8_t iterationsDone;

void setup() {
  b.begin();

  iterationsDone = 0;
  state = STATE_WAIT;
  wasStartPausePressed = false;
  endMillis = 0;
  resetHoldEnd = 0;
  resetSoundWasPlayed = false;
}

void loop() {
  if (b.buttonOn(3)) wasStartPausePressed = true;
  if (b.buttonOn(1)) {
    if (resetHoldEnd == 0) resetHoldEnd = millis() + RESET_HOLD_TIME;
    else if (millis() > resetHoldEnd) {
      state = STATE_WAIT;
      iterationsDone = 0;
      endMillis = 0;
      b.allLedsOff();
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
    case STATE_WAIT: wait(); break;
    case STATE_COUNTING: counting(); break;
    case STATE_REST: rest(); break;
    default:
  }

  if (!b.buttonOn(3)) wasStartPausePressed = false;
}

void wait() {
  showCheckmarks();
  if (wasStartPauseReleased()) {
    state = STATE_COUNTING;
    iterationsDone %= 4;
    b.playNote("C6", 16);
  }
}

void counting() {
  if (endMillis < 1) {
    endMillis = millis() + POMODORO_TIME;
  }

  float progress = (endMillis - millis()) / POMODORO_TIME;
  if (progress < 0) {
    state = STATE_REST;
    endMillis = 0;
    iterationsDone++;
    b.playNote("C5", 16);
    b.playNote("R", 16);
    b.playNote("C5", 8);
    return;
  }

  float ledProgress = progress * 11;
  for (uint8_t led = 1; led <= floor(ledProgress); led++) {
    b.ledOn(led, 255, 0, 0);
  }

  uint8_t lastLed = (uint8_t) ceil(ledProgress);
  float lastLedProgress = ledProgress - floor(ledProgress);
  uint8_t lastLedRedness = (uint8_t)round(lastLedProgress * 255);
  b.ledOn(lastLed, lastLedRedness, 0, 0);

  for (uint8_t led = lastLed+1; led <= 11; led++) {
    b.ledOff(led);
  }
}

void rest() {
  const float restTime = iterationsDone < 4 ? NORMAL_REST_TIME : LONG_REST_TIME;
  if (endMillis < 1) endMillis = millis() + restTime;

  const float progress = (endMillis - millis()) / restTime;
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
    if (
      led == 10 && iterationsDone >= 1 ||
      led == 11 && iterationsDone >= 2 ||
      led == 1 && iterationsDone >= 3 ||
      led == 2 && iterationsDone >= 4
    ) continue;
    b.ledOn(led, colorProgress, colorProgress, colorProgress);
  }
  showCheckmarks();
}

void showCheckmarks() {
  for (uint8_t i = iterationsDone; i > 0; i--) {
    if (i == 1) b.ledOn(10, 0, 255, 0);
    if (i == 2) b.ledOn(11, 0, 255, 0);
    if (i == 3) b.ledOn(1, 0, 255, 0);
    if (i == 4) b.ledOn(2, 0, 255, 0);
  }
}

bool wasStartPauseReleased() {
  return !b.buttonOn(3) && wasStartPausePressed;
}
