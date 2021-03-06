#include "dhcpclient.h"

void MilkyDHCPClient::init(unsigned long timeout, unsigned long responseTimeout) {
  _dhcpLeaseTime=0;
  _dhcpT1=0;
  _dhcpT2=0;
  _lastCheck=0;
  _timeout = timeout;
  _responseTimeout = responseTimeout;

  // zero out _dhcpMacAddr
  memset(_dhcpMacAddr, 0, 6); 
  // zero out _dhcpSubnetMask, _dhcpGatewayIp, _dhcpLocalIp, _dhcpDhcpServerIp, _dhcpDnsServerIp
  memset(_dhcpLocalIp, 0, 20);

  memcpy((void*)_dhcpMacAddr, (void*)SystemConfig.mac, 6);
  _dhcp_state = STATE_DHCP_START;
}

//return:0 on error, 1 if request is sent and response is received
int MilkyDHCPClient::requestDhcpLease(){
  uint8_t messageType = 0;
  
  // Pick an initial transaction ID
  _dhcpTransactionId = random(1UL, 2000UL);
  _dhcpInitialTransactionId = _dhcpTransactionId;
 
  vTaskSuspendAll();
  _dhcpUdpSocket.stop();
  
  if (_dhcpUdpSocket.begin(DHCP_CLIENT_PORT) == 0) {
    // Couldn't get a socket
    xTaskResumeAll();
    return 0;
  }  
  xTaskResumeAll();
  
  int result = 0;
    
  unsigned long startTime = millis();
    
  while (_dhcp_state != STATE_DHCP_LEASED) {
    //vTaskDelay((100L * configTICK_RATE_HZ) / 1000L);
    if(_dhcp_state == STATE_DHCP_START) {
        _dhcpTransactionId++;
        sendDhcpMessage(DHCP_DISCOVER, ((millis() - startTime) / 1000));
        _dhcp_state = STATE_DHCP_DISCOVER;
    } else if (_dhcp_state == STATE_DHCP_REREQUEST) {
      _dhcpTransactionId++;
        sendDhcpMessage(DHCP_REQUEST, ((millis() - startTime)/1000));
        _dhcp_state = STATE_DHCP_REQUEST;
    } else if (_dhcp_state == STATE_DHCP_DISCOVER) {
        uint32_t respId;
        messageType = parseDhcpResponse(_responseTimeout, respId);
        if (messageType == DHCP_OFFER) {
          // We'll use the transaction ID that the offer came with,
          // rather than the one we were up to
          _dhcpTransactionId = respId;
          sendDhcpMessage(DHCP_REQUEST, ((millis() - startTime) / 1000));
          _dhcp_state = STATE_DHCP_REQUEST;
        }
    } else if (_dhcp_state == STATE_DHCP_REQUEST) {
      uint32_t respId;
      messageType = parseDhcpResponse(_responseTimeout, respId);
      if (messageType == DHCP_ACK) {
          _dhcp_state = STATE_DHCP_LEASED;
          result = 1;
          //use default lease time if we didn't get it
          if (_dhcpLeaseTime == 0) {
              _dhcpLeaseTime = DEFAULT_LEASE;
          }
          //calculate T1 & T2 if we didn't get it
          if (_dhcpT1 == 0) {
              //T1 should be 50% of _dhcpLeaseTime
              _dhcpT1 = _dhcpLeaseTime >> 1;
          }
          if (_dhcpT2 == 0) {
              //T2 should be 87.5% (7/8ths) of _dhcpLeaseTime
              _dhcpT2 = _dhcpT1 << 1;
          }
          _renewInSec = _dhcpT1;
          _rebindInSec = _dhcpT2;
      } else if(messageType == DHCP_NAK) {
        _dhcp_state = STATE_DHCP_START;
      }
    }
    
    if (messageType == 255) {
      messageType = 0;
      _dhcp_state = STATE_DHCP_START;
    }
    
    if(result != 1 && ((millis() - startTime) > _timeout))
      break;
  }
  
  // We're done with the socket now  
  vTaskSuspendAll();
  _dhcpUdpSocket.stop();
  xTaskResumeAll();
  _dhcpTransactionId++;

  return result;
}

void MilkyDHCPClient::sendDhcpMessage(uint8_t messageType, uint16_t secondsElapsed) {
  vTaskSuspendAll();
  uint8_t buffer[32];
  memset(buffer, 0, 32);
  IPAddress dest_addr( 255, 255, 255, 255 ); // Broadcast address

  if (-1 == _dhcpUdpSocket.beginPacket(dest_addr, DHCP_SERVER_PORT)) {
      // FIXME Need to return errors
      xTaskResumeAll();
      return;
  }

  buffer[0] = DHCP_BOOTREQUEST;   // op
  buffer[1] = DHCP_HTYPE10MB;     // htype
  buffer[2] = DHCP_HLENETHERNET;  // hlen
  buffer[3] = DHCP_HOPS;          // hops

  // xid
  unsigned long xid = htonl(_dhcpTransactionId);
  memcpy(buffer + 4, &(xid), 4);

  // 8, 9 - seconds elapsed
  buffer[8] = ((secondsElapsed & 0xff00) >> 8);
  buffer[9] = (secondsElapsed & 0x00ff);

  // flags - the only one flag DHCP uses is
  // to request the server response as broadcast, not unicast
//    unsigned short flags = htons(DHCP_FLAGSBROADCAST);
//    memcpy(buffer + 10, &(flags), 2);

  // ciaddr: already zeroed
  // yiaddr: already zeroed
  // siaddr: already zeroed
  // giaddr: already zeroed

  _dhcpUdpSocket.write(buffer, 28);

  memset(buffer, 0, 32); // clear local buffer

  memcpy(buffer, _dhcpMacAddr, 6); // chaddr

  _dhcpUdpSocket.write(buffer, 16);

  memset(buffer, 0, 32); // clear local buffer

  // leave zeroed out for sname && file

  for (int i = 0; i < 6; i++) {
      _dhcpUdpSocket.write(buffer, 32);
  }

  // OPT - Magic Cookie
  buffer[0] = (uint8_t)((MAGIC_COOKIE >> 24)& 0xFF);
  buffer[1] = (uint8_t)((MAGIC_COOKIE >> 16)& 0xFF);
  buffer[2] = (uint8_t)((MAGIC_COOKIE >> 8)& 0xFF);
  buffer[3] = (uint8_t)(MAGIC_COOKIE& 0xFF);

  // OPT - message type
  buffer[4] = dhcpMessageType;
  buffer[5] = 0x01;
  buffer[6] = messageType; //DHCP_REQUEST;

  // OPT - client identifier
  buffer[7] = dhcpClientIdentifier;
  buffer[8] = 0x07;
  buffer[9] = 0x01;
  memcpy(buffer + 10, _dhcpMacAddr, 6);

  // OPT - host name
  buffer[16] = hostName;
  buffer[17] = 12; //len
  strncpy((char*)&(buffer[18]), SystemConfig.parameters.systemName, 12);

  _dhcpUdpSocket.write(buffer, 30);

  if (messageType == DHCP_REQUEST) {
    buffer[0] = dhcpRequestedIPaddr;
    buffer[1] = 0x04;
    buffer[2] = _dhcpLocalIp[0];
    buffer[3] = _dhcpLocalIp[1];
    buffer[4] = _dhcpLocalIp[2];
    buffer[5] = _dhcpLocalIp[3];

    buffer[6] = dhcpServerIdentifier;
    buffer[7] = 0x04;
    buffer[8] = _dhcpDhcpServerIp[0];
    buffer[9] = _dhcpDhcpServerIp[1];
    buffer[10] = _dhcpDhcpServerIp[2];
    buffer[11] = _dhcpDhcpServerIp[3];

    _dhcpUdpSocket.write(buffer, 12);
  }
  
  buffer[0] = dhcpParamRequest;
  buffer[1] = 0x06;
  buffer[2] = subnetMask;
  buffer[3] = routersOnSubnet;
  buffer[4] = dns;
  buffer[5] = domainName;
  buffer[6] = dhcpT1value;
  buffer[7] = dhcpT2value;
  buffer[8] = endOption;
  
  _dhcpUdpSocket.write(buffer, 9);
  _dhcpUdpSocket.endPacket();
  xTaskResumeAll();
}

uint8_t MilkyDHCPClient::parseDhcpResponse(unsigned long responseTimeout, uint32_t &transactionId) {

  uint8_t type = 0;
  uint8_t opt_len = 0;
   
  unsigned long startTime = millis();

  int parseResult = 0;
  do {
    if ((millis() - startTime) > responseTimeout) return 255;
    
    vTaskSuspendAll();
    parseResult = _dhcpUdpSocket.parsePacket();
    xTaskResumeAll();

    vTaskDelay((50L * configTICK_RATE_HZ) / 1000L);
  } while (parseResult <= 0);
   
  // start reading in the packet
  RIP_MSG_FIXED fixedMsg;
  
  vTaskSuspendAll();
  _dhcpUdpSocket.read((uint8_t*)&fixedMsg, sizeof(RIP_MSG_FIXED));
  xTaskResumeAll();

  if (fixedMsg.op == DHCP_BOOTREPLY && _dhcpUdpSocket.remotePort() == DHCP_SERVER_PORT) {
    transactionId = ntohl(fixedMsg.xid);
    if (memcmp(fixedMsg.chaddr, _dhcpMacAddr, 6) != 0 || (transactionId < _dhcpInitialTransactionId) || (transactionId > _dhcpTransactionId)) {
      // Need to read the rest of the packet here regardless
      vTaskSuspendAll();
      _dhcpUdpSocket.discardReceived();
      xTaskResumeAll();
      return 0;
    }

    memcpy(_dhcpLocalIp, fixedMsg.yiaddr, 4);

    // Skip to the option part
    // Doing this a byte at a time so we don't have to put a big buffer
    // on the stack (as we don't have lots of memory lying around)
    for (int i =0; i < (240 - (int)sizeof(RIP_MSG_FIXED)); i++) {
      vTaskSuspendAll();
      _dhcpUdpSocket.read(); // we don't care about the returned byte
      xTaskResumeAll();
    }

    vTaskSuspendAll();
    while (_dhcpUdpSocket.available() > 0) {
      switch (_dhcpUdpSocket.read()) {
        case endOption :
          break;
            
        case padOption :
          break;
        
        case dhcpMessageType :
          opt_len = _dhcpUdpSocket.read();
          type = _dhcpUdpSocket.read();
          break;
      
        case subnetMask :
          opt_len = _dhcpUdpSocket.read();
          _dhcpUdpSocket.read(_dhcpSubnetMask, 4);
          break;
        
        case routersOnSubnet :
          opt_len = _dhcpUdpSocket.read();
          _dhcpUdpSocket.read(_dhcpGatewayIp, 4);
          for (int i = 0; i < opt_len-4; i++) {
              _dhcpUdpSocket.read();
          }
          break;
        
        case dns :
          opt_len = _dhcpUdpSocket.read();
          _dhcpUdpSocket.read(_dhcpDnsServerIp, 4);
          for (int i = 0; i < opt_len-4; i++) {
              _dhcpUdpSocket.read();
          }
          break;
        
        case dhcpServerIdentifier :
          opt_len = _dhcpUdpSocket.read();
          if ( _dhcpDhcpServerIp[0] == 0 || IPAddress(_dhcpDhcpServerIp) == _dhcpUdpSocket.remoteIP()) {
            _dhcpUdpSocket.read(_dhcpDhcpServerIp, sizeof(_dhcpDhcpServerIp));
          } else {
            // Skip over the rest of this option
            while (opt_len--) {
                _dhcpUdpSocket.read();
            }
          }
          break;

        case dhcpT1value : 
          opt_len = _dhcpUdpSocket.read();
          _dhcpUdpSocket.read((uint8_t*)&_dhcpT1, sizeof(_dhcpT1));
          _dhcpT1 = ntohl(_dhcpT1);
          break;

        case dhcpT2value : 
          opt_len = _dhcpUdpSocket.read();
          _dhcpUdpSocket.read((uint8_t*)&_dhcpT2, sizeof(_dhcpT2));
          _dhcpT2 = ntohl(_dhcpT2);
          break;

        case dhcpIPaddrLeaseTime :
          opt_len = _dhcpUdpSocket.read();
          _dhcpUdpSocket.read((uint8_t*)&_dhcpLeaseTime, sizeof(_dhcpLeaseTime));
          _dhcpLeaseTime = ntohl(_dhcpLeaseTime);
          _renewInSec = _dhcpLeaseTime;
          break;

        default :
          opt_len = _dhcpUdpSocket.read();
          // Skip over the rest of this option
          while (opt_len--) {
              _dhcpUdpSocket.read();
          }
          break;
      }
    }
    xTaskResumeAll();
  }

  // Need to skip to end of the packet regardless here
  vTaskSuspendAll();
  _dhcpUdpSocket.discardReceived();
  xTaskResumeAll();
  return type;
}

/*
    returns:
    0/DHCP_CHECK_NONE: nothing happened
    1/DHCP_CHECK_RENEW_FAIL: renew failed
    2/DHCP_CHECK_RENEW_OK: renew success
    3/DHCP_CHECK_REBIND_FAIL: rebind fail
    4/DHCP_CHECK_REBIND_OK: rebind success
*/
int MilkyDHCPClient::checkLease() {
  //this uses a signed / unsigned trick to deal with millis overflow
  unsigned long now = millis();
  signed long snow = (long)now;
  int rc=DHCP_CHECK_NONE;
  if (_lastCheck != 0) {
    signed long factor;
    //calc how many ms past the timeout we are
    factor = snow - (long)_secTimeout;
    //if on or passed the timeout, reduce the counters
    if (factor >= 0) {
      //next timeout should be now plus 1000 ms minus parts of second in factor
      _secTimeout = snow + 1000 - factor % 1000;
      //how many seconds late are we, minimum 1
      factor = factor / 1000 +1;
      
      //reduce the counters by that mouch
      //if we can assume that the cycle time (factor) is fairly constant
      //and if the remainder is less than cycle time * 2 
      //do it early instead of late
      if(_renewInSec < factor*2)
        _renewInSec = 0;
      else
        _renewInSec -= factor;
      
      if(_rebindInSec < factor*2)
        _rebindInSec = 0;
      else
        _rebindInSec -= factor;
    }

    //if we have a lease but should renew, do it
    if (_dhcp_state == STATE_DHCP_LEASED && _renewInSec <=0) {
      _dhcp_state = STATE_DHCP_REREQUEST;
      rc = 1 + requestDhcpLease();
    }

    //if we have a lease or is renewing but should bind, do it
    if ((_dhcp_state == STATE_DHCP_LEASED || _dhcp_state == STATE_DHCP_START) && _rebindInSec <=0) {
      //this should basically restart completely
      _dhcp_state = STATE_DHCP_START;
      // zero out _dhcpSubnetMask, _dhcpGatewayIp, _dhcpLocalIp, _dhcpDhcpServerIp, _dhcpDnsServerIp
      memset(_dhcpLocalIp, 0, 20);
      rc = 3 + requestDhcpLease();
    }
  } else {
    _secTimeout = snow + 1000;
  }

  _lastCheck = now;
  return rc;
}

byte *MilkyDHCPClient::getLocalIP()   {return _dhcpLocalIp;}
byte *MilkyDHCPClient::getDnsIP()     {return _dhcpDnsServerIp;}
byte *MilkyDHCPClient::getGatewayIP() {return _dhcpGatewayIp;}
byte *MilkyDHCPClient::getNetmask()   {return _dhcpSubnetMask;}

MilkyDHCPClient DHCPClient;
