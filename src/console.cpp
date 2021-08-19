#include "console.h"

bool MilkyConsole::init() {
  portBASE_TYPE threadConsole = xTaskCreate(Console.task, "Console", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
  return true;
}

void MilkyConsole::task(void *arg) {
  //Wait up to 5 seconds for console to get ready, otherwise do not initialize it.
  for (uint8_t i = 0; i < 5; i++) {
    if (CONSOLE_DEVICE) {
      CONSOLE_DEVICE.begin(CONSOLE_BAUDRATE);
    }
    vTaskDelay((1000L * configTICK_RATE_HZ) / 1000L);
  }   

  vTaskDelay((500L * configTICK_RATE_HZ) / 1000L);

  String commandBuffer = "";
  String resultBuffer = "";

  CONSOLE_DEVICE.println("MilkyCore "__CORE_VERSION__" built "__DATE__" "__TIME__);
  CONSOLE_DEVICE.print("FreeRTOS Kernel ");
  CONSOLE_DEVICE.println(tskKERNEL_VERSION_NUMBER);
  CONSOLE_DEVICE.print(SystemConfig.parameters.systemName);
  CONSOLE_DEVICE.print("> ");

  while (true) {
    if (CONSOLE_DEVICE.available()) {
      char c = CONSOLE_DEVICE.read();
      CONSOLE_DEVICE.write(c);
      if (c == '\r' || c == '\n') {
        while (CONSOLE_DEVICE.available()) {
          CONSOLE_DEVICE.read();
          vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);            
        }
        resultBuffer = CommandHandler.handleCommand(commandBuffer);
        resultBuffer.replace("\n", "\r\n");

        CONSOLE_DEVICE.print(resultBuffer);
        commandBuffer = "";
        CONSOLE_DEVICE.print(SystemConfig.parameters.systemName);
        CONSOLE_DEVICE.print("> ");
      } else if (c == '\b') {
        commandBuffer.remove(commandBuffer.length() - 1, 1);
        CONSOLE_DEVICE.write(0x7F);
      } else {
        commandBuffer += c;
      }
    } else {
      vTaskDelay((10L * configTICK_RATE_HZ) / 1000L);
    }
  }
}

MilkyConsole Console;