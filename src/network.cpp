#include "network.h"

#ifdef ENABLE_ETHERNET
  bool MilkyNetworkHelper::init() {
    portBASE_TYPE threadEthernetCore;

    Ethernet.init(PB15, PB14, PB13, PB12);  // Most Arduino shields
    //Ethernet.init(PA7, PA6, PA5, PA4);
    Ethernet.begin(SystemConfig.mac, nullAddress, nullAddress, nullAddress, nullAddress);

    //-- check if we have an ethernet controller --
    if (Ethernet.hardwareStatus() == EthernetNoHardware) return false;
    SystemLogger.writeLine(F("[ethernet] Controller found"));

    #ifdef ETHERNET_ENABLE_DHCP
      DHCPClient.init(60000, 4000); //Init dhcp client
    #endif

    threadEthernetCore = xTaskCreate(ethernetTaskCore, "Ethernet Core", configMINIMAL_STACK_SIZE+200, NULL, 2, NULL);

    #ifdef ETHERNET_ENABLE_HTTP
      HTTPServer.init();
    #endif

    #ifdef ETHERNET_ENABLE_TELNET
      TelnetServer.init();
    #endif

    SystemLogger.writeLine(F("[ethernet] Tasks created"));

    return true;
  }

  void MilkyNetworkHelper::reconfigureIP() {
    #ifdef ETHERNET_ENABLE_DHCP
      bool dhcpSuccess = false;
      usingDHCP = false;

      if (SystemConfig.parameters.enableDHCP) {
        SystemLogger.writeLine(F("[ethernet] Trying to get IP Address using DHCP..."));
        dhcpSuccess = DHCPClient.requestDhcpLease() == 1;

        if (dhcpSuccess) {
          SystemLogger.writeLine(F("[ethernet] Configuring dynamic IP address..."));
          
          Ethernet.begin(SystemConfig.mac, 
                         byteArrayToIP(DHCPClient.getLocalIP()), 
                         byteArrayToIP(DHCPClient.getDnsIP()), 
                         byteArrayToIP(DHCPClient.getGatewayIP()), 
                         byteArrayToIP(DHCPClient.getNetmask()));
          isEthernetReady = true;
          usingDHCP = true;
                         
          SystemLogger.writeLine(F("[ethernet] Dynamic IP address set"));
        }
      }

      if (!SystemConfig.parameters.enableDHCP || (SystemConfig.parameters.enableDHCP && SystemConfig.parameters.allowStaticFallback && !dhcpSuccess)) {
        if (SystemConfig.parameters.enableDHCP)
          SystemLogger.writeLine(F("[ethernet] DHCP failed! Using static fallback"));
        configStaticIP();
      }
    #else
      configStaticIP();
    #endif
  }

  void MilkyNetworkHelper::configStaticIP() {
    SystemLogger.writeLine(F("[ethernet] Configuring static IP address..."));
    
    Ethernet.begin(SystemConfig.mac, 
                   byteArrayToIP(SystemConfig.parameters.device), 
                   byteArrayToIP(SystemConfig.parameters.dns), 
                   byteArrayToIP(SystemConfig.parameters.gateway), 
                   byteArrayToIP(SystemConfig.parameters.netmask));
    isEthernetReady = true;
                 
    SystemLogger.writeLine(F("[ethernet] Static IP address set"));
  }

  bool MilkyNetworkHelper::getIsLinkUp() {
    return currentLinkStatus == LinkON;
  }

  bool MilkyNetworkHelper::getIsEthernetReady() {
    return isEthernetReady;
  }

  bool MilkyNetworkHelper::getIsUsingDHCP() {
    return usingDHCP;
  }

  void MilkyNetworkHelper::ethernetTaskCore(void *arg) {
    //Wait a bit before running
    vTaskDelay((2000L * configTICK_RATE_HZ) / 1000L);
    
    EthernetLinkStatus lastLinkStatus = Unknown;
    IPAddress lastKnownIP = NetworkHelper.nullAddress;
    while (true) {
      vTaskSuspendAll();
      Ethernet.maintain();
      NetworkHelper.currentLinkStatus = Ethernet.linkStatus();
      xTaskResumeAll();

      if (NetworkHelper.currentLinkStatus != lastLinkStatus) {
        lastLinkStatus = NetworkHelper.currentLinkStatus;
        SystemLogger.write(F("[ethernet] Link status changed to "));
        switch (lastLinkStatus) {
          case Unknown:
            SystemLogger.writeLine(F("Unknown"));
            NetworkHelper.isEthernetReady = false;
            break;
          case LinkON:
            SystemLogger.writeLine(F("UP"));
            NetworkHelper.reconfigureIP();
            break;
          case LinkOFF:
            SystemLogger.writeLine(F("DOWN"));
            NetworkHelper.isEthernetReady = false;
            break;
        }
      }

      if (NetworkHelper.wasEthernetReady != NetworkHelper.isEthernetReady) {
        NetworkHelper.wasEthernetReady = NetworkHelper.isEthernetReady;
        if (NetworkHelper.isEthernetReady) {
          SystemLogger.writeLine(F("[ethernet] Ethernet is now available"));
          NetworkHelper.restartAllServerTasks();
        } else {
          SystemLogger.writeLine(F("[ethernet] Ethernet is no longer available"));
        }
      }

      #ifdef ETHERNET_ENABLE_DHCP
        if (NetworkHelper.currentLinkStatus == LinkON && SystemConfig.parameters.enableDHCP) DHCPClient.checkLease();
      #endif
        
      #ifdef ETHERNET_ENABLE_NTP
        if (NetworkHelper.currentLinkStatus == LinkON && SystemConfig.parameters.enableNTP) NTPClient.tick();
      #endif
      
      if (Ethernet.localIP() != lastKnownIP) {
        lastKnownIP = Ethernet.localIP();
        SystemLogger.write(F("[ethernet] Got new IP Address: "));
        SystemLogger.writeLine(NetworkHelper.ipAddressToString(lastKnownIP));
      }

      vTaskDelay((1000L * configTICK_RATE_HZ) / 1000L);
    }
  }

  void MilkyNetworkHelper::restartAllServerTasks() {
    #ifdef ETHERNET_ENABLE_HTTP
      HTTPServer.restart();
    #endif
    #ifdef ETHERNET_ENABLE_TELNET
      TelnetServer.restart();
    #endif
  }

  IPAddress MilkyNetworkHelper::byteArrayToIP(byte address[4]) {
    return IPAddress(address[0], address[1], address[2], address[3]);
  }
  
  byte *MilkyNetworkHelper::ipStringToByteArray(String address) {
    static byte out[4];
    sscanf(address.c_str(), "%d.%d.%d.%d", out, out + 1, out + 2, out + 3);
    return out;
  }
  
  String MilkyNetworkHelper::ipAddressToString(IPAddress ipAddress) {
    String s;
    for (byte i = 0; i < 4; ++i)
    {
      char buf[4];
      sprintf(buf, "%d", ipAddress[i]);
      s += buf;
      if (i < 3) s += '.';
    }
    return s;
  }
  
  String MilkyNetworkHelper::macAddressToString() {
    String s;
    for (byte i = 0; i < 6; ++i)
    {
      char buf[4];
      sprintf(buf, "%02x", SystemConfig.mac[i]);
      s += buf;
      if (i < 5) s += ':';
    }
    return s;
  }

#endif

MilkyNetworkHelper NetworkHelper;