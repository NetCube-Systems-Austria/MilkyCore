#ifndef NETWORK_H
#define NETWORK_H

#include "MilkyCore.h"

class MilkyNetworkHelper {
  public:
    #ifdef ENABLE_ETHERNET
      const IPAddress nullAddress = IPAddress(0,0,0,0);
      bool init();
      void reconfigureIP();
      void configStaticIP();
      bool getIsLinkUp();
      bool getIsUsingDHCP();
      bool getIsEthernetReady();
      static void ethernetTaskCore(void *arg);
      void restartAllServerTasks();
      IPAddress byteArrayToIP(byte address[4]);
      byte *ipStringToByteArray(String address);
      String ipAddressToString(IPAddress ipAddress);
    #endif

  private:
    EthernetLinkStatus currentLinkStatus = Unknown;
    bool wasEthernetReady = false;
    bool isEthernetReady = false;
    bool usingDHCP = false;
};

extern MilkyNetworkHelper NetworkHelper;

#endif