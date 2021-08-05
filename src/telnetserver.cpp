#include "httpserver.h"

#if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_HTTP)
  void MilkyTelnetServer::init() {
    portBASE_TYPE threadEthernetTelnet = xTaskCreate(TelnetServer.task, "Telnet Server", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
  }

  void MilkyTelnetServer::task(void *arg) {
    vTaskDelay((2000L * configTICK_RATE_HZ) / 1000L);
    while (true) {
      TelnetServer.server.begin();
      SystemLogger.writeLine(F("[ telnet ] Server started!"));
    
      for (int i = 0; i < HTTP_MAX_CLIENT_THREADS; i++) {
        TelnetServer.clients[i] = EthernetClient();
      }
    
      while (TelnetServer.server) {
        for (int i = 0; i < HTTP_MAX_CLIENT_THREADS; i++) {
          vTaskSuspendAll();
          bool isValid = (TelnetServer.clients[i] == true);
          xTaskResumeAll();
          if (!isValid) {
            vTaskSuspendAll();
            TelnetServer.clients[i] = TelnetServer.server.accept();
            isValid = (TelnetServer.clients[i] == true);
            xTaskResumeAll();
            if (isValid) {
              SystemLogger.writeLine(F("[ telnet ] New client! Creating task..."));
              vTaskDelay((10L * configTICK_RATE_HZ) / 1000L);
              
              portBASE_TYPE threadClientHandler = xTaskCreate(TelnetServer.clientHandlerTask, "Telnet Client Handler", configMINIMAL_STACK_SIZE+200, (void*)i, 4, NULL);
              
              SystemLogger.write(F("[ telnet ] Telnet Client Task "));
              SystemLogger.write(String(i));
              SystemLogger.writeLine(F(" created!"));
              vTaskDelay((10L * configTICK_RATE_HZ) / 1000L);
            } else {
              vTaskDelay((500L * configTICK_RATE_HZ) / 1000L);
            }
          }
          vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);
        }
        
        vTaskDelay((10L * configTICK_RATE_HZ) / 1000L);
      }
      SystemLogger.writeLine(F("[ telnet ] Server restarting..."));
      vTaskDelay((2000L * configTICK_RATE_HZ) / 1000L);
    }
  }

  void MilkyTelnetServer::restart() {
    server.end();
  }

  void MilkyTelnetServer::clientHandlerTask(void *i) {
    int clientIndex = (int)i;
    String commandBuffer = "";
    bool connected = true;
    
    vTaskSuspendAll();
    TelnetServer.clients[clientIndex].println("MilkyCore "__CORE_VERSION__" built "__DATE__" "__TIME__);
    TelnetServer.clients[clientIndex].print("FreeRTOS Kernel ");
    TelnetServer.clients[clientIndex].println(tskKERNEL_VERSION_NUMBER);
    TelnetServer.clients[clientIndex].print(SystemConfig.parameters.systemName);
    TelnetServer.clients[clientIndex].print("> ");
    xTaskResumeAll();
    
    while (connected) {
      vTaskSuspendAll();
      int avail = TelnetServer.clients[clientIndex].available();
      connected = TelnetServer.clients[clientIndex].connected();
      xTaskResumeAll();
      if (avail) {
        vTaskSuspendAll();
        char c = TelnetServer.clients[clientIndex].read();
        if (c == '\r' || c == '\n') {
          while (TelnetServer.clients[clientIndex].available()) {
            TelnetServer.clients[clientIndex].read();
            vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);            
          }
          TelnetServer.handleCommand(commandBuffer, TelnetServer.clients[clientIndex]);
          commandBuffer = "";
          TelnetServer.clients[clientIndex].print(SystemConfig.parameters.systemName);
          TelnetServer.clients[clientIndex].print("> ");
        } else {
          commandBuffer += c;
        }
        xTaskResumeAll();
      } else {
        vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);
      }
    }
    
    SystemLogger.write(F("[ telnet ] Telnet Client Task "));
    SystemLogger.write(String(clientIndex));
    SystemLogger.writeLine(F(" ended!"));
    TelnetServer.clients[clientIndex] = EthernetClient();
    vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);
    vTaskDelete( NULL );  
  }

  void MilkyTelnetServer::handleCommand(String command, EthernetClient client) {
    int actionSeperator = command.indexOf(' ');
    int subcommandSeperator = command.indexOf(' ', actionSeperator + 1);
    String action = command.substring(0, actionSeperator);
    String subcommand = "";
    String value = "";
    
    if (actionSeperator > 0)
      subcommand = command.substring(actionSeperator + 1, subcommandSeperator);

    if (subcommandSeperator > 0)
      value = command.substring(subcommandSeperator + 1);
    
    if (action.equals("exit")) {
      client.stop();
    } else if (action.equals("uptime")) {
      client.println("System uptime: " + String(millis()/1000) + "s");
    } else if (action.equals("version")) {
      client.println("MilkyCore "__CORE_VERSION__" built "__DATE__" "__TIME__);
      client.print("FreeRTOS Kernel ");
      client.println(tskKERNEL_VERSION_NUMBER);
    } else if (action.equals("netconfig")) {
      if (subcommand == "" || subcommand == "show") {     
        client.println("");
        client.println("===============================");
        client.println("==   Network Configuration   ==");
        client.println("===============================");
        client.print("           Enable DHCP: "); client.println(SystemConfig.parameters.enableDHCP ? "Yes" : "No");
        client.print(" Allow Static Fallback: "); client.println(SystemConfig.parameters.allowStaticFallback ? "Yes" : "No");
        client.print("             Device IP: "); client.println(NetworkHelper.byteArrayToIP(SystemConfig.parameters.device));
        client.print("               Netmask: "); client.println(NetworkHelper.byteArrayToIP(SystemConfig.parameters.netmask));
        client.print("               Gateway: "); client.println(NetworkHelper.byteArrayToIP(SystemConfig.parameters.gateway));
        client.print("            Nameserver: "); client.println(NetworkHelper.byteArrayToIP(SystemConfig.parameters.dns));
        client.println("");
      } else if (subcommand == "set") {
        int keySep = value.indexOf('=') + 1;
        String key = value.substring(0, keySep - 1); key.toLowerCase();

        if (key.equals("enabledhcp")) SystemConfig.parameters.enableDHCP = value.substring(keySep).equalsIgnoreCase("yes");
        else if (key.equals("allowstaticfallback")) SystemConfig.parameters.allowStaticFallback = value.substring(keySep).equalsIgnoreCase("yes");
        else if (key.equals("deviceip")) memcpy(SystemConfig.parameters.device, NetworkHelper.ipStringToByteArray(value.substring(keySep)), 4);
        else if (key.equals("netmask")) memcpy(SystemConfig.parameters.netmask, NetworkHelper.ipStringToByteArray(value.substring(keySep)), 4);
        else if (key.equals("gateway")) memcpy(SystemConfig.parameters.gateway, NetworkHelper.ipStringToByteArray(value.substring(keySep)), 4);
        else if (key.equals("nameserver")) memcpy(SystemConfig.parameters.dns, NetworkHelper.ipStringToByteArray(value.substring(keySep)), 4);
        else client.println("Unknown key or invalid usage.\n Valid keys: 'enabledhcp', 'allowstaticfallback', 'deviceip', 'netmask', 'gateway', 'nameserver'\n Usage: key=value\n Example: deviceip=192.168.0.50");
        SystemConfig.writeConfigToMemory();
      } else {
        client.println("Unknown subcommand '" + subcommand + "'! Available subcommands: 'show', 'set'");
      }
    } else if (action.equals("sysconfig")) {
      if (subcommand == "" || subcommand == "show") {
        client.println("");
        client.println("===============================");
        client.println("==   System  Configuration   ==");
        client.println("===============================");
        client.print("           System Name: "); client.println(SystemConfig.parameters.systemName);
        client.print("       Reboot On Crash: "); client.println(SystemConfig.parameters.rebootOnCrash ? "Yes" : "No");
        client.print("  Screen Saver Timeout: "); client.println(SystemConfig.parameters.screenSaverTimeout);
        client.println("");
      } else if (subcommand == "set") {
        int keySep = value.indexOf('=') + 1;
        String key = value.substring(0, keySep - 1); key.toLowerCase();

        if (key.equals("systemname")) snprintf(SystemConfig.parameters.systemName, sizeof(SystemConfig.parameters.systemName), "%s", value.substring(keySep).c_str());
        else if (key.equals("rebootoncrash")) SystemConfig.parameters.rebootOnCrash = value.substring(keySep).equalsIgnoreCase("yes");
        else if (key.equals("screensavertimeout")) SystemConfig.parameters.screenSaverTimeout = value.substring(keySep).toInt();
        else client.println("Unknown key or invalid usage.\n Valid keys: 'systemname', 'rebootoncrash', 'screensavertimeout'\n Usage: key=value\n Example: systemname=Fritz");
        SystemConfig.writeConfigToMemory();
      }else {
        client.println("Unknown subcommand '" + subcommand + "'! Available subcommands: 'show', 'set'");
      }
    } 
    #ifdef ENABLE_RTC
      else if (action.equals("timeconfig")) {
        if (subcommand == "" || subcommand == "show") {
          client.println("");
          client.println("===============================");
          client.println("== Time & Date Configuration ==");
          client.println("===============================");
          client.print("            Enable NTP: "); client.println(SystemConfig.parameters.enableNTP ? "Yes" : "No");
          client.print("                  Time: "); client.println(SystemRTC.getCurrentTimeStr());
          client.print("                  Date: "); client.println(SystemRTC.getCurrentDateStr());
          client.print(" Daylight Savings Time: "); client.println(SystemConfig.parameters.enableDST ? "Yes" : "No");
          client.print("       Timezone Offset: "); client.println(SystemConfig.parameters.timezoneOffset);
          client.println("");
        } else if (subcommand == "set") {
          int keySep = value.indexOf('=') + 1;
          String key = value.substring(0, keySep - 1); key.toLowerCase();
  
          if (key.equals("time")) SystemRTC.setTimeFromString(value.substring(keySep));
          else if (key.equals("date")) SystemRTC.setDateFromString(value.substring(keySep));
          else if (key.equals("enablentp")) SystemConfig.parameters.enableNTP = value.substring(keySep).equalsIgnoreCase("yes");
          else if (key.equals("dst")) SystemConfig.parameters.enableDST = value.substring(keySep).equalsIgnoreCase("yes");
          else if (key.equals("timezoneoffset")) SystemConfig.parameters.timezoneOffset = value.substring(keySep).toInt();
          else client.println("Unknown key or invalid usage.\n Valid keys: 'time', 'date', 'enablentp', 'dst', 'timezoneoffset'\n Usage: key=value\n Example: time=18:06:32");
          SystemConfig.writeConfigToMemory();
        } else {
          client.println("Unknown subcommand '" + subcommand + "'! Available subcommands: 'show', 'set'");
        }
      }
    #endif
    else if (action.equals("netstatus")) {
      client.println("");
      client.println("===============================");
      client.println("==      Network  Status      ==");
      client.println("===============================");
      client.print("            Using DHCP: "); client.println(NetworkHelper.getIsUsingDHCP() ? "Yes" : "No");
      client.print("             Device IP: "); client.println(Ethernet.localIP());
      client.print("               Netmask: "); client.println(Ethernet.subnetMask());
      client.print("            Gateway IP: "); client.println(Ethernet.gatewayIP());
      client.print("         Nameserver IP: "); client.println(Ethernet.dnsServerIP());
      client.println("");
    } else if (action.equals("reset")) {
      client.println("Board will be reset in 5 seconds!");
      client.stop();
      WatchdogHelper.resetBoard();
    } else if (action.equals("fortestingpurposesonly")) {
      while (true);
      client.stop();
      WatchdogHelper.resetBoard();
    } else if (action.equals("")) {
      client.println();
    } else if (action.equals("help")) {
      client.println("");
      client.println("===============================");
      client.println("==   MilkyCore Telnet Help   ==");
      client.println("===============================");
      client.println("       exit - End telnet session");
      client.println("     uptime - Show time passed since last reset/reboot");
      client.println("    version - Show firmware and kernel version");
      client.println("  sysconfig - Show and edit system configuration");
      client.println("  appconfig - Show and edit app configuration");
      client.println(" timeconfig - Show and edit RTC/NTP configuration");
      client.println("  netconfig - Show and edit network configuration");
      client.println("  netstatus - Show active network configuration");
      client.println("      reset - Force reboot device");
      client.println("       help - Show this help");
      client.println("");
    } else {
      client.println("Unknown command '" + action + "'. Type 'help' to show available commands.");
    }
  }
#endif

MilkyTelnetServer TelnetServer;