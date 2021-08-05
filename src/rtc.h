#ifndef RTC_H
#define RTC_H

#include "MilkyCore.h"

#ifdef ENABLE_RTC
  #include <time.h>
#endif  

class MilkyRTC {
  public:
    #ifdef ENABLE_RTC  
      bool init();
      void setEpochTime(unsigned long epoch);
      unsigned long getEpochTime();
      void setTimeFromString(String timeStr);
      void setDateFromString(String dateStr);
      char *getCurrentDateStr();
      char *getCurrentShortDateStr();
      char *getCurrentTimeStr();
    #endif  

  private:
    #ifdef ENABLE_RTC
      STM32RTC& rtc = STM32RTC::getInstance();
    #endif  
};

extern MilkyRTC SystemRTC;

#endif
