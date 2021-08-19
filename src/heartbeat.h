#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "MilkyCore.h"

#ifndef HEARTBEAT_LED
  #error "Heartbeat LED Pin is not defined! Please define HEARTBEAT_LED"
#endif

class MilkyHeartbeatHelper {
  public:
      static void task(void *arg);
  private:

};

extern MilkyHeartbeatHelper HeartbeatHelper;

#endif