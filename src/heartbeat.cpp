#include "heartbeat.h"

void MilkyHeartbeatHelper::task(void *arg) {
  while (true) {
    digitalWrite(HEARTBEAT_LED, HIGH);
    vTaskDelay((1000L * configTICK_RATE_HZ) / 1000L);
    digitalWrite(HEARTBEAT_LED, LOW);
    vTaskDelay((1000L * configTICK_RATE_HZ) / 1000L);
  }
}

MilkyHeartbeatHelper HeartbeatHelper;