#include "watchdog.h"

void MilkyWatchdogHelper::resetBoard() {
  setSoftwareWatchdogEnabled(false);
  SystemLogger.writeLine(F("Resetting board in 5 sec..."));
  IWatchdog.begin(5000000);
  while (true) {};
}

void MilkyWatchdogHelper::task(void *arg) {
  WatchdogHelper.hardwareWatchdogInit();
  WatchdogHelper.softwareWatchdogInit();
    
  while (true) {
    vTaskDelay((1000L * configTICK_RATE_HZ) / 1000L);
      WatchdogHelper.hardwareWatchdogTick();
      WatchdogHelper.softwareWatchdogTick();
   }
}

void MilkyWatchdogHelper::earlyHardwareWatchdogInit() {
  IWatchdog.begin(IWDG_TIMEOUT_MAX);
  SystemLogger.write(F("[watchdog] Early hardware watchdog now enabled ("));
  SystemLogger.write(String(IWDG_TIMEOUT_MAX / 1000000));
  SystemLogger.writeLine(F(" sec)"));
}

void MilkyWatchdogHelper::hardwareWatchdogInit() {
  IWatchdog.reload();
  IWatchdog.begin(10000000);
  SystemLogger.writeLine(F("[watchdog] Hardware watchdog now enabled (10 sec)"));
}

void MilkyWatchdogHelper::hardwareWatchdogTick() {
  IWatchdog.reload();
}

void MilkyWatchdogHelper::softwareWatchdogBark(void) {
  if (!WatchdogHelper.softwareWatchdogEnabled) return;
  
  UBaseType_t uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
  
  WatchdogHelper.softwareWatchdog->pause();
  IWatchdog.reload();
  TaskHandle_t handle = xTaskGetCurrentTaskHandle();
  
  digitalWrite(ERROR_LED, HIGH);

  for (int i = 0; i < 2; i++) SystemLogger.writeLine();
  SystemLogger.writeLine(F(" --== Software Watchdog Triggered! ==-- "));
  SystemLogger.writeLine();
  SystemLogger.write(F(" Task \"")); SystemLogger.write(pcTaskGetName(handle)); SystemLogger.writeLine(F("\" took too long to give back control."));
  for (int i = 0; i < 2; i++) SystemLogger.writeLine();

  if (SystemConfig.parameters.rebootOnCrash) {
    WatchdogHelper.resetBoard();
  } else {
    WatchdogHelper.softwareWatchdogEnabled = false;
    //And now we wait... For the guy to come fix us... Or at least reset the board 
    while (true) {
      IWatchdog.reload();
      delay(100);
    };
  }
  
  taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );
}

void MilkyWatchdogHelper::earlySoftwareWatchdogInit() {
  //Early Software Watchdog not supported
}

void MilkyWatchdogHelper::softwareWatchdogInit() {
  SystemLogger.writeLine(F("[watchdog] Configuring software watchdog..."));
  softwareWatchdog->pause();
  softwareWatchdog->setCount(0, MICROSEC_FORMAT);
  softwareWatchdog->setOverflow(1000 * 1000 * 5, MICROSEC_FORMAT); // 5 Sec
  softwareWatchdog->attachInterrupt(WatchdogHelper.softwareWatchdogBark);
  softwareWatchdog->resume();

  vTaskDelay((1000L * configTICK_RATE_HZ) / 1000L);
  
  softwareWatchdogEnabled = true;
  SystemLogger.writeLine(F("[watchdog] Software watchdog now enabled (5 sec)"));
}

void MilkyWatchdogHelper::softwareWatchdogTick() {
  softwareWatchdog->setCount(0, MICROSEC_FORMAT);
}

void MilkyWatchdogHelper::setSoftwareWatchdogEnabled(bool enabled) {
  softwareWatchdogEnabled = enabled;
}

MilkyWatchdogHelper WatchdogHelper;