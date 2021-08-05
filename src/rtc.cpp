#include "rtc.h"

#ifdef ENABLE_RTC
  bool MilkyRTC::init() {
    STM32RTC& rtc = STM32RTC::getInstance();
    rtc.setClockSource(STM32RTC::LSE_CLOCK);
    rtc.begin();
    
    return true;
  }
  
  void MilkyRTC::setEpochTime(unsigned long epoch) {
    rtc.setEpoch(epoch);
  }
  
  unsigned long MilkyRTC::getEpochTime() {
    return rtc.getEpoch() + (SystemConfig.parameters.timezoneOffset * 3600) + (SystemConfig.parameters.enableDST ? 3600 : 0);
  }
  
  void MilkyRTC::setTimeFromString(String timeStr) {
    time_t t = getEpochTime();
    tm* dt = gmtime(&t);
    sscanf(timeStr.c_str(), "%d:%d:%d", dt->tm_hour, dt->tm_min, dt->tm_sec);
    t = mktime(dt);
    setEpochTime(t - (SystemConfig.parameters.timezoneOffset * 3600) + (SystemConfig.parameters.enableDST ? 3600 : 0));
  }
  
  void MilkyRTC::setDateFromString(String dateStr) {
    time_t t = getEpochTime();
    tm* dt = gmtime(&t);
    sscanf(dateStr.c_str(), "%d.%d.%d", dt->tm_mday, dt->tm_mon, dt->tm_year);
    dt->tm_mon -= 1;
    dt->tm_year -= 1900;
    t = mktime(dt);
    setEpochTime(t - (SystemConfig.parameters.timezoneOffset * 3600) + (SystemConfig.parameters.enableDST ? 3600 : 0));
  }
  
  char *MilkyRTC::getCurrentDateStr() {
    static char buffer[12];
    time_t t = getEpochTime();
    tm* dt = gmtime(&t);
    sprintf(buffer, "%02d.%02d.%04d", dt->tm_mday, dt->tm_mon + 1, dt->tm_year + 1900);
    return buffer;
  }
  
  char *MilkyRTC::getCurrentShortDateStr() {
    static char buffer[7];
    time_t t = getEpochTime();
    tm* dt = gmtime(&t);
    sprintf(buffer, "%02d.%02d.", dt->tm_mday, dt->tm_mon + 1);
    return buffer;
  }
  
  char *MilkyRTC::getCurrentTimeStr() {
    static char buffer[6];
    time_t t = getEpochTime();
    tm* dt = gmtime(&t);
    sprintf(buffer, "%02d:%02d", dt->tm_hour, dt->tm_min);
    return buffer;
  }
#endif  

MilkyRTC SystemRTC;