#ifndef DHCPCLIENT_H
#define DHCPCLIENT_H

#include "MilkyCore.h"

#if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_DHCP)
  #include "utility/util.h"
#endif

class MilkyDHCPClient {
  public:
    #if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_DHCP)
      void init(unsigned long timeout, unsigned long responseTimeout);
      int requestDhcpLease();
      int checkLease();
      byte *getLocalIP();
      byte *getDnsIP();
      byte *getGatewayIP();
      byte *getNetmask();
    #endif

  private:
    #if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_DHCP)
      void sendDhcpMessage(uint8_t messageType, uint16_t secondsElapsed);
      uint8_t parseDhcpResponse(unsigned long responseTimeout, uint32_t &transactionId);
      uint32_t _dhcpInitialTransactionId;
      uint32_t _dhcpTransactionId;
      uint8_t  _dhcpMacAddr[6];
      uint8_t  _dhcpLocalIp[4];
      uint8_t  _dhcpSubnetMask[4];
      uint8_t  _dhcpGatewayIp[4];
      uint8_t  _dhcpDhcpServerIp[4];
      uint8_t  _dhcpDnsServerIp[4];
      uint32_t _dhcpLeaseTime;
      uint32_t _dhcpT1, _dhcpT2;
      signed long _renewInSec;
      signed long _rebindInSec;
      signed long _lastCheck;
      unsigned long _timeout;
      unsigned long _responseTimeout;
      unsigned long _secTimeout;
      uint8_t _dhcp_state;
      EthernetUDP _dhcpUdpSocket;
    #endif
};

extern MilkyDHCPClient DHCPClient;

#endif