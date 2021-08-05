#include "MilkyCore.h"

void MilkyCore::init(void) {
  SystemLogger.init();
  SystemLogger.writeLine(F("\n\n--------------------------------"));
  SystemLogger.write(F("[  main  ] Firmware build ")); SystemLogger.write(__DATE__); SystemLogger.write(" "); SystemLogger.writeLine(__TIME__);
  SystemLogger.write(F("[  main  ] Firmware version ")); SystemLogger.writeLine(__CORE_VERSION__); 
  SystemLogger.write(F("[  main  ] FreeRTOS Kernel ")); SystemLogger.writeLine(tskKERNEL_VERSION_NUMBER);
  SystemLogger.write(F("[  main  ] Starting up...")); SystemLogger.writeLine();

  #ifdef ENABLE_HARDWARE_WATCHDOG
    WatchdogHelper.earlyHardwareWatchdogInit();
  #endif
  
  #ifdef ENABLE_SOFTWARE_WATCHDOG
    WatchdogHelper.earlySoftwareWatchdogInit();
  #endif

  SystemConfig.configurationMemoryInit();
  SystemConfig.readMacAddressFromMemory();
  SystemConfig.readConfigFromMemory();

  SystemLogger.write(F("[  main  ] System Name: ")); SystemLogger.writeLine(SystemConfig.getSystemName());

  portBASE_TYPE taskSystemHelper = xTaskCreate(systemHelperTask, "System Helper", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  SystemLogger.writeLine(F("[  main  ] System Helper task created"));

  #ifdef ENABLE_HEARTBEAT_LED
    portBASE_TYPE taskHeartbeat = xTaskCreate(HeartbeatHelper.task, "Heartbeat", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    SystemLogger.writeLine(F("[  main  ] Heartbeat task created"));
  #endif

  #if defined(ENABLE_HARDWARE_WATCHDOG) || defined(ENABLE_SOFTWARE_WATCHDOG)
    portBASE_TYPE taskWatchdog = xTaskCreate(WatchdogHelper.task, "Watchdog", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    SystemLogger.writeLine(F("[  main  ] Watchdog task created"));
  #endif

  #ifdef ENABLE_RTC
    if (SystemRTC.init()) {
      SystemLogger.writeLine(F("[  main  ] RTC initialized"));
    }
  #endif

  #ifdef ENABLE_ETHERNET
    if (NetworkHelper.init()) {
      SystemLogger.writeLine(F("[  main  ] Ethernet initialized"));
    }
  #endif

  portBASE_TYPE threadUserAppMain = xTaskCreate(userAppMain, "User App", configMINIMAL_STACK_SIZE+400, NULL, 1, NULL);
  SystemLogger.writeLine(F("[  main  ] User task created"));

  SystemLogger.writeLine(F("[  main  ] Starting task scheduler..."));
  vTaskStartScheduler();
  
  for (int i = 0; i < 2; i++) SystemLogger.writeLine();
  SystemLogger.writeLine(F(" --== Task Scheduler Exited! ==-- "));
  for (int i = 0; i < 2; i++) SystemLogger.writeLine();
  WatchdogHelper.resetBoard();
}

void MilkyCore::userAppMain(void *arg) {
  vTaskDelay((5000L * configTICK_RATE_HZ) / 1000L);

  SystemLogger.writeLine(F("[  user  ] Starting user application..."));

  //setup();

  while (true) {
    loop();
    vTaskDelay((1000L * configTICK_RATE_HZ) / 1000L);
  }
}

void MilkyCore::systemHelperTask(void *arg) {
  while (true) {
    serialEventRun();
    #if defined(CORE_CALLBACK)
      CoreCallback();
    #endif
    vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);
  }
}

MilkyCore Core;