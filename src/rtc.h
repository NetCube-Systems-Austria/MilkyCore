#ifndef RTC_H
#define RTC_H

#include "MilkyCore.h"
#include <time.h>

class MilkyRTC {
  public:
    bool init();
    void setEpochTime(unsigned long epoch);
    unsigned long getEpochTime();
    void setTimeFromString(String timeStr);
    void setDateFromString(String dateStr);
    char *getCurrentDateStr();
    char *getCurrentShortDateStr();
    char *getCurrentTimeStr();

  private:
    STM32RTC& rtc = STM32RTC::getInstance();
};

extern MilkyRTC SystemRTC;

#endif
