#include "network.h"

bool MilkyNetworkHelper::init() {
  portBASE_TYPE threadEthernetCore;

  //Check if board has Ethernet Chipset
  pinMode(ETH_CS, INPUT_PULLUP);
  if (digitalRead(ETH_CS) == LOW) return false; //Jumper is set to disable ethernet

  Ethernet.init(ETH_MOSI, ETH_MISO, ETH_SCK, ETH_CS);  // Most Arduino shields
  //Ethernet.init(PA7, PA6, PA5, PA4);
  Ethernet.begin(SystemConfig.mac, nullAddress, nullAddress, nullAddress, nullAddress);

  //-- check if we have an ethernet controller --
  if (Ethernet.hardwareStatus() == EthernetNoHardware) return false;
  SystemLogger.writeLine(F("[ethernet] Controller found"));

  DHCPClient.init(60000, 4000); //Init dhcp client

  threadEthernetCore = xTaskCreate(ethernetTaskCore, "Ethernet Core", configMINIMAL_STACK_SIZE+200, NULL, 2, NULL);

  HTTPServer.init();
  TelnetServer.init();

  SystemLogger.writeLine(F("[ethernet] Tasks created"));

  hasEthernet = true;

  return true;
}

void MilkyNetworkHelper::reconfigureIP() {
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

bool MilkyNetworkHelper::getHasEthernet() {
  return hasEthernet;
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

    if (NetworkHelper.currentLinkStatus == LinkON && SystemConfig.parameters.enableDHCP)
      DHCPClient.checkLease();
    
    if (NetworkHelper.currentLinkStatus == LinkON && SystemConfig.parameters.enableNTP)
      NTPClient.tick();
    
    if (Ethernet.localIP() != lastKnownIP) {
      lastKnownIP = Ethernet.localIP();
      SystemLogger.write(F("[ethernet] Got new IP Address: "));
      SystemLogger.writeLine(NetworkHelper.ipAddressToString(lastKnownIP));
    }

    vTaskDelay((1000L * configTICK_RATE_HZ) / 1000L);
  }
}

void MilkyNetworkHelper::restartAllServerTasks() {
  HTTPServer.restart();
  TelnetServer.restart();
}

IPAddress MilkyNetworkHelper::byteArrayToIP(byte address[4]) {
  return IPAddress(address[0], address[1], address[2], address[3]);
}

byte *MilkyNetworkHelper::ipStringToByteArray(String address) {
  static byte out[4];
  sscanf(address.c_str(), "%d.%d.%d.%d", out, out + 1, out + 2, out + 3);
  return out;
}

String MilkyNetworkHelper::ipAddressToString(byte address[4]) {
  String s;
  for (byte i = 0; i < 4; ++i)
  {
    char buf[4];
    sprintf(buf, "%d", address[i]);
    s += buf;
    if (i < 3) s += '.';
  }
  return s;
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


MilkyNetworkHelper NetworkHelper;