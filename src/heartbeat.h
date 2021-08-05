#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "MilkyCore.h"

#ifdef ENABLE_HEARTBEAT_LED
  #ifndef HEARTBEAT_LED
    #error "Heartbeat LED Pin is not defined! Please define HEARTBEAT_LED"
  #endif
#endif

class MilkyHeartbeatHelper {
  public:
    #ifdef ENABLE_HEARTBEAT_LED
      static void task(void *arg);
    #endif
  private:

};

extern MilkyHeartbeatHelper HeartbeatHelper;

#endif