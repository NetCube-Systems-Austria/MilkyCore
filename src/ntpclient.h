#ifndef NTPCLIENT_H
#define NTPCLIENT_H

#include "MilkyCore.h"

#define SEVENZYYEARS 2208988800UL
#define NTP_PACKET_SIZE 48
#define NTP_CLIENT_PORT 1337
#define NTP_PORT 123

#if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_NTP)
  #ifndef ENABLE_RTC
    #error "NTP can not work without RTC enabled. Please define ENABLE_RTC"
  #endif
#endif  

class MilkyNTPClient {
  public:
    #if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_NTP)
      void tick();
    #endif
  private:
    #if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_NTP)
      int readNTPPacket();
      void sendNTPPacket();
      EthernetUDP _ntpUdpSocket;
      bool _udpSetup       = false;
      const char* _poolServerName = "pool.ntp.org"; // Default time server
      long _timeOffset     = 0;
      unsigned long _currentEpoc    = 0;      // In s
      unsigned long _lastUpdate     = 0;      // In ms
      byte _packetBuffer[NTP_PACKET_SIZE];
    #endif
};

extern MilkyNTPClient NTPClient;

#endif