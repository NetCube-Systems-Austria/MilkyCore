#include "logger.h"

void MilkyLogger::init() {
  //Wait up to 5 seconds for logger to get ready, otherwise do not initialize it.
  for (uint8_t i = 0; i < 5; i++) {
    if (LOG_DEVICE) {
      LOG_DEVICE.begin(LOG_BAUDRATE); 
      return;
    }
    delay(1000);
  }   
}

void MilkyLogger::write(String value) {
  if (!LOG_DEVICE) return;
  LOG_DEVICE.print(value);
}

void MilkyLogger::writeLine(String value) {
  if (!LOG_DEVICE) return;
  LOG_DEVICE.println(value);
}

void MilkyLogger::writeLine() {
  if (!LOG_DEVICE) return;
  LOG_DEVICE.println();
}

MilkyLogger SystemLogger;