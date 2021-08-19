#ifndef CONSOLE_H
#define CONSOLE_H

#include "MilkyCore.h"

#ifndef CONSOLE_DEVICE
  #error "Console Device is not defined! Please define CONSOLE_DEVICE"
#endif

#ifndef CONSOLE_BAUDRATE
  #warning "Console Baudrate not defined. Using default of 115200 baud"
  #define CONSOLE_BAUDRATE 115200
#endif

class MilkyConsole {
  public:
    bool init();

  private:
    static void task(void *arg);
};

extern MilkyConsole Console;

#endif