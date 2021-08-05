#include "logger.h"

void MilkyLogger::init() {
  #ifdef ENABLE_LOGGING
  //Wait up to 5 seconds for logger to get ready, otherwise do not initialize it.
    for (uint8_t i = 0; i < 5; i++) {
      if (LOG_DEVICE) {
        LOG_DEVICE.begin(LOG_BAUDRATE); 
        return;
      }
      delay(1000);
    }    
  #endif
}

void MilkyLogger::write(String value) {
  #ifdef ENABLE_LOGGING
    if (!LOG_DEVICE) return;
    LOG_DEVICE.print(value);
  #endif
}

void MilkyLogger::writeLine(String value) {
  #ifdef ENABLE_LOGGING
    if (!LOG_DEVICE) return;
    LOG_DEVICE.println(value);
  #endif
}

void MilkyLogger::writeLine() {
  #ifdef ENABLE_LOGGING
    if (!LOG_DEVICE) return;
    LOG_DEVICE.println();
  #endif
}

MilkyLogger SystemLogger;