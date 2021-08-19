#ifndef MILKYCORE_H
#define MILKYCORE_H

#define __CORE_VERSION__ "V0.3.4"

#define WMS_REV_B

#define HTTP_MAX_CLIENT_THREADS 8
#define HTTP_CLIENT_TIMEOUT 8
#define HTTP_PORT 80

#define TELNET_MAX_CLIENT_THREADS 8
#define TELNET_PORT 23

#include <Arduino.h>
#include <STM32FreeRTOS.h>
#include <SPI.h>
#include <Wire.h>
#include <STM32RTC.h>
#include <EthernetENC.h>
#include <EthernetUdp.h>
#include <HttpRequest.h>

#include "boarddefs.h"
#include "logger.h"
#include "configuration.h"
#include "watchdog.h"
#include "heartbeat.h"
#include "rtc.h"
#include "gpio.h"
#include "network.h"
#include "dhcpclient.h"
#include "ntpclient.h"
#include "httpserver.h"
#include "telnetserver.h"
#include "console.h"
#include "commandhandler.h"

#define _min(a,b) (((a)<(b))?(a):(b))
#define _max(a,b) (((a)>(b))?(a):(b))

class MilkyCore {
  public:
    void earlyInit();
    void init();
  private:
    static void userAppMain(void *arg);
    static void systemHelperTask(void *arg);
    static String coreCommandHandler(String command);
};

extern MilkyCore Core;

#endif
