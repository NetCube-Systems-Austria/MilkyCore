#ifndef MILKYCORE_H
#define MILKYCORE_H

#define __CORE_VERSION__ "V0.3.0"

#define ENABLE_HARDWARE_WATCHDOG
#define ENABLE_SOFTWARE_WATCHDOG

#define ENABLE_HEARTBEAT_LED
#define HEARTBEAT_LED PB8
#define ERROR_LED PB9

#define I2C1_SDA PB7
#define I2C1_SCL PB6

#define ENABLE_RTC

#define ENABLE_LOGGING
#define LOG_DEVICE SerialUSB
#define LOG_BAUDRATE 115200

#define ENABLE_ETHERNET
#define ETHERNET_ENABLE_DHCP
#define ETHERNET_ENABLE_NTP

#define ETHERNET_ENABLE_HTTP
#define HTTP_MAX_CLIENT_THREADS 8
#define HTTP_CLIENT_TIMEOUT 8
#define HTTP_PORT 80

#define ETHERNET_ENABLE_TELNET
#define TELNET_MAX_CLIENT_THREADS 8
#define TELNET_PORT 23

#include <Arduino.h>
#include <STM32FreeRTOS.h>
#include <SPI.h>
#include <Wire.h>

#ifdef ENABLE_RTC
  #include <STM32RTC.h>
#endif

#ifdef ENABLE_ETHERNET
  #include <EthernetENC.h>
  #if defined(ETHERNET_ENABLE_DHCP) || defined(ETHERNET_ENABLE_NTP)
    #include <EthernetUdp.h>
  #endif
  #ifdef ETHERNET_ENABLE_HTTP
    #include <HttpRequest.h>
  #endif
#endif

#include "logger.h"
#include "configuration.h"
#include "watchdog.h"
#include "heartbeat.h"
#include "rtc.h"
#include "network.h"
#include "dhcpclient.h"
#include "ntpclient.h"
#include "httpserver.h"
#include "telnetserver.h"

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

class MilkyCore {
  public:
    void init();
  private:
    static void userAppMain(void *arg);
    static void systemHelperTask(void *arg);
};

extern MilkyCore Core;

#endif
