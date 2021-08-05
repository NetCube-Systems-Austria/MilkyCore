#include "ntpclient.h"

#if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_NTP)
  void MilkyNTPClient::tick() {
    if ((millis() - _lastUpdate >= (SystemConfig.parameters.updateInterval * 60000)) || _lastUpdate == 0) {
      sendNTPPacket();
      readNTPPacket();
    }
  }

  int MilkyNTPClient::readNTPPacket() {
    vTaskSuspendAll();
    _ntpUdpSocket.stop();
    
    if (_ntpUdpSocket.begin(NTP_CLIENT_PORT) == 0) {
      // Couldn't get a socket
      xTaskResumeAll();
      return 0;
    }  
    xTaskResumeAll();
    
    // Wait till data is there or timeout...
    byte timeout = 0;
    int cb = 0;
    do {
      vTaskDelay((10L * configTICK_RATE_HZ) / 1000L);
      vTaskSuspendAll();
      cb = _ntpUdpSocket.parsePacket();
      xTaskResumeAll();
      if (timeout > 100) return 0; // timeout after 1000 ms
      timeout++;
    } while (cb == 0);

    _lastUpdate = millis() - (10 * (timeout + 1)); // Account for delay in reading the time

    vTaskSuspendAll();
    _ntpUdpSocket.read(_packetBuffer, NTP_PACKET_SIZE);
    xTaskResumeAll();

    unsigned long highWord = word(_packetBuffer[40], _packetBuffer[41]);
    unsigned long lowWord = word(_packetBuffer[42], _packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    _currentEpoc = secsSince1900 - SEVENZYYEARS;

    SystemRTC.setEpochTime(_currentEpoc);

    return 1;
  }

  void MilkyNTPClient::sendNTPPacket() {
    // set all bytes in the buffer to 0
    memset(_packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    _packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    _packetBuffer[1] = 0;     // Stratum, or type of clock
    _packetBuffer[2] = 6;     // Polling Interval
    _packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    _packetBuffer[12]  = 49;
    _packetBuffer[13]  = 0x4E;
    _packetBuffer[14]  = 49;
    _packetBuffer[15]  = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    vTaskSuspendAll();
    _ntpUdpSocket.beginPacket(_poolServerName, NTP_PORT); //NTP requests are to port 123
    _ntpUdpSocket.write(_packetBuffer, NTP_PACKET_SIZE);
    _ntpUdpSocket.endPacket();
    xTaskResumeAll();
  }
#endif

MilkyNTPClient NTPClient;