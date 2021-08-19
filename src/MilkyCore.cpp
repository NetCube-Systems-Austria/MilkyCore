#include "MilkyCore.h"

void MilkyCore::earlyInit(void) {
  pinMode(HEARTBEAT_LED, OUTPUT);

  #ifdef ERROR_LED
    pinMode(ERROR_LED, OUTPUT);
  #endif

  digitalWrite(HEARTBEAT_LED, HIGH);
  
  #ifdef ERROR_LED
    digitalWrite(ERROR_LED, HIGH);
  #endif

  SystemLogger.init();

  #ifdef ERROR_LED
    digitalWrite(ERROR_LED, LOW);
  #endif

  SystemLogger.write(F("\f"));
  SystemLogger.write(F("[  main  ] Firmware build ")); SystemLogger.write(__DATE__); SystemLogger.write(" "); SystemLogger.writeLine(__TIME__);
  SystemLogger.write(F("[  main  ] Firmware version ")); SystemLogger.writeLine(__CORE_VERSION__); 
  SystemLogger.write(F("[  main  ] FreeRTOS Kernel ")); SystemLogger.writeLine(tskKERNEL_VERSION_NUMBER);

  SystemLogger.writeLine(F("[  main  ] Early system initialisation..."));

  Wire.setSDA(I2C1_SDA);
  Wire.setSCL(I2C1_SCL);
  Wire.begin();
  SystemLogger.writeLine(F("[  main  ] I2C bus initialized"));

  SystemConfig.configurationMemoryInit();
  SystemConfig.readMacAddressFromMemory();
  SystemConfig.readConfigFromMemory();

  SystemLogger.write(F("[  main  ] MAIN BD: ")); SystemLogger.writeLine(SystemConfig.macAddressToString(SystemConfig.mac));
  SystemLogger.write(F("[  main  ] System Name: ")); SystemLogger.writeLine(SystemConfig.getSystemName());
  
  GPIO.init();
  SystemLogger.writeLine(F("[  main  ] Local GPIO initialized"));

  SystemLogger.writeLine(F("[  main  ] Early system initialisation done."));
}

void MilkyCore::init(void) {
  SystemLogger.write(F("[  main  ] Starting up...")); SystemLogger.writeLine();

  WatchdogHelper.earlyHardwareWatchdogInit();
  WatchdogHelper.earlySoftwareWatchdogInit();

  portBASE_TYPE taskSystemHelper = xTaskCreate(systemHelperTask, "System Helper", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  SystemLogger.writeLine(F("[  main  ] System Helper task created"));

  portBASE_TYPE taskHeartbeat = xTaskCreate(HeartbeatHelper.task, "Heartbeat", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
  SystemLogger.writeLine(F("[  main  ] Heartbeat task created"));

  portBASE_TYPE taskWatchdog = xTaskCreate(WatchdogHelper.task, "Watchdog", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
  SystemLogger.writeLine(F("[  main  ] Watchdog task created"));

  if (SystemRTC.init()) {
    SystemLogger.writeLine(F("[  main  ] RTC initialized"));
  }

  if (NetworkHelper.init()) {
    SystemLogger.writeLine(F("[  main  ] Ethernet initialized"));
  }

  if (Console.init()) {
    SystemLogger.writeLine(F("[  main  ] Console initialized"));
  }
  
  CommandHandler.registerUserCommandHandler(&MilkyCore::coreCommandHandler);

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
    vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);
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

String MilkyCore::coreCommandHandler(String command) {
  int actionSeperator = command.indexOf(' ');
  int subcommandSeperator = command.indexOf(' ', actionSeperator + 1);
  String action = command.substring(0, actionSeperator);
  String args = command.substring(actionSeperator + 1);
  String subcommand = "";
  String value = "";
  String result = "";

  if (actionSeperator > 0)
  subcommand = command.substring(actionSeperator + 1, subcommandSeperator);

  if (subcommandSeperator > 0)
  value = command.substring(subcommandSeperator + 1);

  if (action.equals("uptime")) {
    result += "System uptime: " + String(millis()/1000) + "s\n";
  } else if (action.equals("version")) {
    result += "MilkyCore "__CORE_VERSION__" built "__DATE__" "__TIME__"\n";
    result += "FreeRTOS Kernel "tskKERNEL_VERSION_NUMBER"\n";
  } else if (action.equals("netconfig") && NetworkHelper.getHasEthernet()) {
    if (subcommand == "" || subcommand == "show") {     
      result += "\n";
      result += "===============================\n";
      result += "==   Network Configuration   ==\n";
      result += "===============================\n";
      result += "           Enable DHCP: "; result += (SystemConfig.parameters.enableDHCP ? "Yes" : "No"); result += "\n";
      result += " Allow Static Fallback: "; result += (SystemConfig.parameters.allowStaticFallback ? "Yes" : "No"); result += "\n";
      result += "             Device IP: "; result += (NetworkHelper.ipAddressToString(SystemConfig.parameters.device)); result += "\n";
      result += "               Netmask: "; result += (NetworkHelper.ipAddressToString(SystemConfig.parameters.netmask)); result += "\n";
      result += "               Gateway: "; result += (NetworkHelper.ipAddressToString(SystemConfig.parameters.gateway)); result += "\n";
      result += "            Nameserver: "; result += (NetworkHelper.ipAddressToString(SystemConfig.parameters.dns)); result += "\n";
      result += "\n";
    } else if (subcommand == "set") {
      int keySep = value.indexOf('=') + 1;
      String key = value.substring(0, keySep - 1); key.toLowerCase();

      if (key.equals("enabledhcp")) {SystemConfig.parameters.enableDHCP = value.substring(keySep).equalsIgnoreCase("yes"); result += "Ok!\n";}
      else if (key.equals("allowstaticfallback")) {SystemConfig.parameters.allowStaticFallback = value.substring(keySep).equalsIgnoreCase("yes"); result += "Ok!\n";}
      else if (key.equals("deviceip")) {memcpy(SystemConfig.parameters.device, NetworkHelper.ipStringToByteArray(value.substring(keySep)), 4); result += "Ok!\n";}
      else if (key.equals("netmask")) {memcpy(SystemConfig.parameters.netmask, NetworkHelper.ipStringToByteArray(value.substring(keySep)), 4); result += "Ok!\n";}
      else if (key.equals("gateway")) {memcpy(SystemConfig.parameters.gateway, NetworkHelper.ipStringToByteArray(value.substring(keySep)), 4); result += "Ok!\n";}
      else if (key.equals("nameserver")) {memcpy(SystemConfig.parameters.dns, NetworkHelper.ipStringToByteArray(value.substring(keySep)), 4); result += "Ok!\n";}
      else result += "Unknown key or invalid usage.\n Valid keys: 'enabledhcp', 'allowstaticfallback', 'deviceip', 'netmask', 'gateway', 'nameserver'\n Usage: key=value\n Example: deviceip=192.168.0.50\n";
      SystemConfig.writeConfigToMemory();
    } else {
      result += "Unknown subcommand '" + subcommand + "'! Available subcommands: 'show', 'set'\n";
    }
  } else if (action.equals("sysconfig")) {
    if (subcommand == "" || subcommand == "show") {
      result += "\n";
      result += "===============================\n";
      result += "==   System  Configuration   ==\n";
      result += "===============================\n";
      result += "           System Name: "; result += (SystemConfig.parameters.systemName); result += "\n";
      result += "       Reboot On Crash: "; result += (SystemConfig.parameters.rebootOnCrash ? "Yes" : "No"); result += "\n";
      result += "\n";
    } else if (subcommand == "set") {
      int keySep = value.indexOf('=') + 1;
      String key = value.substring(0, keySep - 1); key.toLowerCase();

      if (key.equals("systemname")) {snprintf(SystemConfig.parameters.systemName, sizeof(SystemConfig.parameters.systemName), "%s", value.substring(keySep).c_str()); result += "Ok!\n";}
      else if (key.equals("rebootoncrash")) {SystemConfig.parameters.rebootOnCrash = value.substring(keySep).equalsIgnoreCase("yes"); result += "Ok!\n";}
      else if (key.equals("screensavertimeout")) {SystemConfig.parameters.screenSaverTimeout = value.substring(keySep).toInt(); result += "Ok!\n";}
      else result += "Unknown key or invalid usage.\n Valid keys: 'systemname', 'rebootoncrash'\n Usage: key=value\n Example: systemname=Fritz\n";
      SystemConfig.writeConfigToMemory();
    } else {
      result += "Unknown subcommand '" + subcommand + "'! Available subcommands: 'show', 'set'\n";
    }
  } else if (action.equals("timeconfig") && NetworkHelper.getHasEthernet()) {
    if (subcommand == "" || subcommand == "show") {
      result += "\n";
      result += "===============================\n";
      result += "== Time & Date Configuration ==\n";
      result += "===============================\n";
      result += "            Enable NTP: "; result += (SystemConfig.parameters.enableNTP ? "Yes" : "No"); result += "\n";
      result += "                  Time: "; result += (SystemRTC.getCurrentTimeStr()); result += "\n";
      result += "                  Date: "; result += (SystemRTC.getCurrentDateStr()); result += "\n";
      result += " Daylight Savings Time: "; result += (SystemConfig.parameters.enableDST ? "Yes" : "No"); result += "\n";
      result += "       Timezone Offset: "; result += (SystemConfig.parameters.timezoneOffset); result += "\n";
      result += "\n";
    } else if (subcommand == "set") {
      int keySep = value.indexOf('=') + 1;
      String key = value.substring(0, keySep - 1); key.toLowerCase();

      if (key.equals("time")) {SystemRTC.setTimeFromString(value.substring(keySep)); result += "Ok!\n";}
      else if (key.equals("date")) {SystemRTC.setDateFromString(value.substring(keySep)); result += "Ok!\n";}
      else if (key.equals("enablentp")) {SystemConfig.parameters.enableNTP = value.substring(keySep).equalsIgnoreCase("yes"); result += "Ok!\n";}
      else if (key.equals("dst")) {SystemConfig.parameters.enableDST = value.substring(keySep).equalsIgnoreCase("yes"); result += "Ok!\n";}
      else if (key.equals("timezoneoffset")) {SystemConfig.parameters.timezoneOffset = value.substring(keySep).toInt(); result += "Ok!\n";}
      else result += "Unknown key or invalid usage.\n Valid keys: 'time', 'date', 'enablentp', 'dst', 'timezoneoffset'\n Usage: key=value\n Example: time=18:06:32\n";
      SystemConfig.writeConfigToMemory();
    } else {
      result += "Unknown subcommand '" + subcommand + "'! Available subcommands: 'show', 'set'\n";
    }
  } else if (action.equals("timeconfig") && !NetworkHelper.getHasEthernet()) {
    if (subcommand == "" || subcommand == "show") {
      result += "\n";
      result += "===============================\n";
      result += "== Time & Date Configuration ==\n";
      result += "===============================\n";
      result += "                  Time: "; result += (SystemRTC.getCurrentTimeStr()); result += "\n";
      result += "                  Date: "; result += (SystemRTC.getCurrentDateStr()); result += "\n";
      result += " Daylight Savings Time: "; result += (SystemConfig.parameters.enableDST ? "Yes" : "No"); result += "\n";
      result += "       Timezone Offset: "; result += (SystemConfig.parameters.timezoneOffset); result += "\n";
      result += "\n";
    } else if (subcommand == "set") {
      int keySep = value.indexOf('=') + 1;
      String key = value.substring(0, keySep - 1); key.toLowerCase();

      if (key.equals("time")) {SystemRTC.setTimeFromString(value.substring(keySep)); result += "Ok!\n";}
      else if (key.equals("date")) {SystemRTC.setDateFromString(value.substring(keySep)); result += "Ok!\n";}
      else if (key.equals("dst")) {SystemConfig.parameters.enableDST = value.substring(keySep).equalsIgnoreCase("yes"); result += "Ok!\n";}
      else if (key.equals("timezoneoffset")) {SystemConfig.parameters.timezoneOffset = value.substring(keySep).toInt(); result += "Ok!\n";}
      else result += "Unknown key or invalid usage.\n Valid keys: 'time', 'date', 'dst', 'timezoneoffset'\n Usage: key=value\n Example: time=18:06:32\n";
      SystemConfig.writeConfigToMemory();
    } else {
      result += "Unknown subcommand '" + subcommand + "'! Available subcommands: 'show', 'set'\n";
    }
  } else if (action.equals("netstatus") && NetworkHelper.getHasEthernet()) {
    result += "\n";
    result += "===============================\n";
    result += "==      Network  Status      ==\n";
    result += "===============================\n";
    result += "      Hardware Address: "; result += (NetworkHelper.macAddressToString()); result += "\n";
    result += "            Using DHCP: "; result += (NetworkHelper.getIsUsingDHCP() ? "Yes" : "No"); result += "\n";
    result += "             Device IP: "; result += (NetworkHelper.ipAddressToString(Ethernet.localIP())); result += "\n";
    result += "               Netmask: "; result += (NetworkHelper.ipAddressToString(Ethernet.subnetMask())); result += "\n";
    result += "            Gateway IP: "; result += (NetworkHelper.ipAddressToString(Ethernet.gatewayIP())); result += "\n";
    result += "         Nameserver IP: "; result += (NetworkHelper.ipAddressToString(Ethernet.dnsServerIP())); result += "\n";
    result += "\n";
  } else if (action.equals("reboot")) {
    WatchdogHelper.resetBoard();
  } else if (action.equals("fortestingpurposesonly")) {
    while (true);
  } else if (action.equals("__help")) {
    result += "    sysconfig - Show and edit system configuration\n";
    if (NetworkHelper.getHasEthernet()) {
      result += "   timeconfig - Show and edit RTC/NTP configuration\n";
      result += "    netconfig - Show and edit network configuration\n";
      result += "    netstatus - Show active network configuration\n";
    } else {
      result += "   timeconfig - Show and edit RTC configuration\n";
    }
    result += "         help - Show this help\n";
    result += "      version - Show firmware and kernel version\n";
    result += "       uptime - Show time passed since last reset/reboot\n";
    result += "       reboot - Reboot device\n";
  }

  if (result != "") return result;
  return "$$NORES$$";
}

MilkyCore Core;