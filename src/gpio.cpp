#include "gpio.h"

void LocalIO::init() {
  pinMode(GPIO_A0, INPUT_PULLUP);
  pinMode(GPIO_A1, INPUT_PULLUP);
  pinMode(GPIO_A2, INPUT_PULLUP);
  pinMode(GPIO_A3, INPUT_PULLUP);
  pinMode(GPIO_B0, INPUT_PULLUP);
  pinMode(GPIO_B1, INPUT_PULLUP);
  pinMode(GPIO_B2, INPUT_PULLUP);
  pinMode(GPIO_B3, INPUT_PULLUP);
}

uint16_t LocalIO::getAnalog(uint8_t pin) {
  switch (pin) {
    case AI7: return analogRead(GPIO_B2);
    case AI8: return analogRead(GPIO_B3);
    default: return 0;
  }
}

void LocalIO::setAnalog(uint8_t pin, uint16_t state) {

}

uint8_t LocalIO::getDigital(uint8_t pin) {
  switch (pin) {
    case I1: return digitalRead(GPIO_A0);
    case I2: return digitalRead(GPIO_A1);
    case I3: return digitalRead(GPIO_A2);
    case I4: return digitalRead(GPIO_A3);
    case I5: return digitalRead(GPIO_B0);
    case I6: return digitalRead(GPIO_B1);
    case I7: return digitalRead(GPIO_B2);
    case I8: return digitalRead(GPIO_B3);
    case Q1: return expanderGetPin(1);
    case Q2: return expanderGetPin(2);
    case Q3: return expanderGetPin(3);
    case Q4: return expanderGetPin(4);
    case Q5: return expanderGetPin(5);
    case Q6: return expanderGetPin(6);
    default: return 0;
  }
}

void LocalIO::setDigital(uint8_t pin, uint8_t state) {
  switch (pin) {
    case Q1: return expanderSetPin(1, state);
    case Q2: return expanderSetPin(2, state);
    case Q3: return expanderSetPin(3, state);
    case Q4: return expanderSetPin(4, state);
    case Q5: return expanderSetPin(5, state);
    case Q6: return expanderSetPin(6, state);
  }
}

void LocalIO::expanderSetPin(uint8_t pin, uint8_t state) {
  if (state)
    expanderState |= (1 << pin);
  else
    expanderState &= ~(1 << pin);

  vTaskSuspendAll();
  Wire.beginTransmission(EXPANDER_ADDR);
  Wire.write(expanderState);
  Wire.endTransmission();
  xTaskResumeAll();
}

uint8_t LocalIO::expanderGetPin(uint8_t pin) {
  uint8_t data;
  
  vTaskSuspendAll();
  Wire.requestFrom(EXPANDER_ADDR, 1);
  if (Wire.available()) {
    data = Wire.read();
  }
  Wire.endTransmission();
  xTaskResumeAll();

  return (data & (1 << pin)) ? HIGH : LOW;
}

LocalIO GPIO;