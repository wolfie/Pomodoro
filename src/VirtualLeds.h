#include <InternetButton.h>

#ifndef VLEDS_H
#define VLEDS_H

class VirtualLeds {
  public:
    VirtualLeds(const InternetButton& b);

    void loop();

    void ledOn(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
    void ledOff(uint8_t led);

    void allLedsOn(uint8_t r, uint8_t g, uint8_t b);
    void allLedsOff();

    void copyFrom(const VirtualLeds& other);

  private:
    InternetButton b;
    uint8_t vLeds [11][3] = {
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0}
    };
};

#endif /* end of include guard: VLEDS_H */
