#ifndef LOGGER_H
#define LOGGER_H

#include "MilkyCore.h"

#ifdef ENABLE_LOGGING
  #ifndef LOG_DEVICE
    #error "Logging Device is not defined! Please define LOG_DEVICE"
  #endif

  #ifndef LOG_BAUDRATE
    #warning "Logging Baudrate not defined. Using default of 115200 baud"
    #define LOG_BAUDRATE 115200
  #endif
#endif

class MilkyLogger
{
  public: 
    void init();
    void write(String value);
    void writeLine(String value);
    void writeLine();
  private:
};

extern MilkyLogger SystemLogger;

#endif