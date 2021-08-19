#ifndef NETWORK_H
#define NETWORK_H

#include "MilkyCore.h"

class MilkyNetworkHelper {
  public:
    const IPAddress nullAddress = IPAddress(0,0,0,0);
    bool init();
    void reconfigureIP();
    void configStaticIP();
    bool getIsLinkUp();
    bool getIsUsingDHCP();
    bool getIsEthernetReady();
    bool getHasEthernet();
    static void ethernetTaskCore(void *arg);
    void restartAllServerTasks();
    IPAddress byteArrayToIP(byte address[4]);
    byte *ipStringToByteArray(String address);
    String ipAddressToString(IPAddress ipAddress);
    String ipAddressToString(byte address[4]);
    String macAddressToString();

  private:
    EthernetLinkStatus currentLinkStatus = Unknown;
    bool wasEthernetReady = false;
    bool isEthernetReady = false;
    bool usingDHCP = false;
    bool hasEthernet = false;
};

extern MilkyNetworkHelper NetworkHelper;

#endif