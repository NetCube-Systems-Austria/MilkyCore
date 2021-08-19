#include "telnetserver.h"

void MilkyTelnetServer::init() {
  portBASE_TYPE threadEthernetTelnet = xTaskCreate(TelnetServer.task, "Telnet Server", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

void MilkyTelnetServer::task(void *arg) {
  vTaskDelay((2000L * configTICK_RATE_HZ) / 1000L);
  while (true) {
    TelnetServer.server.begin();
    SystemLogger.writeLine(F("[ telnet ] Server started"));
  
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
            SystemLogger.writeLine(F("[ telnet ] New client. Creating task..."));
            vTaskDelay((10L * configTICK_RATE_HZ) / 1000L);
            
            portBASE_TYPE threadClientHandler = xTaskCreate(TelnetServer.clientHandlerTask, "Telnet Client Handler", configMINIMAL_STACK_SIZE+200, (void*)i, 4, NULL);
            
            SystemLogger.write(F("[ telnet ] Telnet Client Task "));
            SystemLogger.write(String(i));
            SystemLogger.writeLine(F(" created"));
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
          //vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);            
        }
        digitalWrite(ERROR_LED, 1);
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
  SystemLogger.writeLine(F(" ended"));
  TelnetServer.clients[clientIndex] = EthernetClient();
  vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);
  vTaskDelete( NULL );  
}

void MilkyTelnetServer::handleCommand(String command, EthernetClient client) {
  if (command.equals("exit")) {
    client.stop();
  } else {
    client.print(CommandHandler.handleCommand(command));
    if (command.equals("help")) {
      client.print("         exit - Exit the telnet session\n");
    }
  }
}

MilkyTelnetServer TelnetServer;