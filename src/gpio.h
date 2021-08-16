#ifndef GPIO_H
#define GPIO_H

#include "MilkyCore.h"

#define GPIO_A0 PA15
#define GPIO_A1 PB3
#define GPIO_A2 PB4
#define GPIO_A3 PB5
#define GPIO_B0 PA8
#define GPIO_B1 PB10
#define GPIO_B2 PB1
#define GPIO_B3 PB2

#define EXPANDER_ADDR 0x20

class LocalIO {
  public:
    void init();

    void setDigital(uint8_t pin, uint8_t state);
    uint8_t getDigital(uint8_t pin);
    void setAnalog(uint8_t pin, uint16_t value);
    uint16_t getAnalog(uint8_t pin);

  private:
    void expanderPinMode(uint8_t pin, uint8_t mode);
    void expanderSetPin(uint8_t pin, uint8_t state);
    uint8_t expanderGetPin(uint8_t pin);
    void expanderUpdate();

    uint8_t expanderDir = 0;
    uint8_t expanderState = 0;
    uint8_t expanderRead = 0;
};

extern LocalIO GPIO;

#endif