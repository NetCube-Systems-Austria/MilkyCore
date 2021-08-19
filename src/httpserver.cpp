#include "httpserver.h"

void MilkyHTTPServer::init() {
  portBASE_TYPE threadEthernetHttp = xTaskCreate(HTTPServer.task, "HTTP Server", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

void MilkyHTTPServer::task(void *arg) {
  vTaskDelay((2000L * configTICK_RATE_HZ) / 1000L);
  while (true) {
    HTTPServer.server.begin();
    SystemLogger.writeLine(F("[  http  ] Server started"));
  
    for (int i = 0; i < HTTP_MAX_CLIENT_THREADS; i++) {
      HTTPServer.clients[i] = EthernetClient();
    }
  
    while (HTTPServer.server) {
      for (int i = 0; i < HTTP_MAX_CLIENT_THREADS; i++) {
        vTaskSuspendAll();
        bool isValid = (HTTPServer.clients[i] == true);
        xTaskResumeAll();
        if (!isValid) {
          vTaskSuspendAll();
          HTTPServer.clients[i] = HTTPServer.server.accept();
          isValid = (HTTPServer.clients[i] == true);
          xTaskResumeAll();
          if (isValid) {
            SystemLogger.writeLine(F("[  http  ] New client. Creating task..."));
            vTaskDelay((10L * configTICK_RATE_HZ) / 1000L);
            
            portBASE_TYPE threadClientHandler = xTaskCreate(HTTPServer.clientHandlerTask, "HTTP Client Handler", configMINIMAL_STACK_SIZE+200, (void*)i, 4, NULL);
            
            SystemLogger.write(F("[  http  ] HTTP Client Task "));
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
    SystemLogger.writeLine(F("[  http  ] Server restarting..."));
    vTaskDelay((2000L * configTICK_RATE_HZ) / 1000L);
  }
}

void MilkyHTTPServer::restart() {
  server.end();
}

void MilkyHTTPServer::clientHandlerTask(void *i) {
  int clientIndex = (int)i;
  bool connected = true;
  int timeout = HTTP_CLIENT_TIMEOUT * 1000;

  while (connected) {    
    vTaskSuspendAll();
    int avail = HTTPServer.clients[clientIndex].available();
    connected = HTTPServer.clients[clientIndex].connected();
    xTaskResumeAll();
    if (avail) {
      vTaskSuspendAll();
      char c = HTTPServer.clients[clientIndex].read();
      HTTPServer.requestHandlers[clientIndex].parseRequest(c);
      xTaskResumeAll();
      
      if (HTTPServer.requestHandlers[clientIndex].endOfRequest()) {
        vTaskSuspendAll();
        HTTPServer.handleHttpRequest(HTTPServer.requestHandlers[clientIndex], HTTPServer.clients[clientIndex]);
        HTTPServer.requestHandlers[clientIndex].resetRequest();
        HTTPServer.clients[clientIndex].flush();
        HTTPServer.clients[clientIndex].stop();
        xTaskResumeAll();
      }
      
      timeout = HTTP_CLIENT_TIMEOUT * 1000;
    }
    
    if (timeout <= 0) {
      SystemLogger.writeLine(F("[  http  ] HTTP Request timed out"));
      vTaskSuspendAll();
      HTTPServer.requestHandlers[clientIndex].resetRequest();
      HTTPServer.clients[clientIndex].stop();
      xTaskResumeAll();
    }

    timeout--;
    vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);
  }
  
  SystemLogger.write(F("[  http  ] HTTP Client Task "));
  SystemLogger.write(String(clientIndex));
  SystemLogger.writeLine(F(" ended"));
  vTaskDelay((1L * configTICK_RATE_HZ) / 1000L);
  vTaskDelete( NULL );  
}

void MilkyHTTPServer::registerUserRequestHandler(uint8_t (*handler)(HttpRequest, EthernetClient)) {
  userRequestHandlers.push_back(handler);
}

void MilkyHTTPServer::unregisterUserRequestHandler(uint8_t (*handler)(HttpRequest, EthernetClient)) {
  userRequestHandlers.remove(handler);
}

void MilkyHTTPServer::handleHttpRequest(HttpRequest request, EthernetClient client) {
  for ( list<UserRequestHandler>::iterator handler = userRequestHandlers.begin(); handler != userRequestHandlers.end(); handler++ ) {
    if ((*handler)(request, client) == 0) return; //If the request was handeled, return.
  }
  if (strcmp(request.uri, "/status.cgi") == 0) {
    sendDefaultStatusPage(client, 200, "OK"); 
  } else {
    sendDefaultStatusPage(client, 404, "Not found"); 
  }
}

void MilkyHTTPServer::sendHttpHeader(EthernetClient client, int statusCode, char *statusText, char *contentType) {
  char buffer[128];
  sprintf(buffer, "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nConnection: close\r\n\r\n", statusCode, statusText, contentType);
  client.print(buffer);
}

void MilkyHTTPServer::sendDefaultStatusPage(EthernetClient client, int statusCode, char *statusText) {
  sendHttpHeader(client, statusCode, statusText, "text/html");
  client.println("<!DOCTYPE HTML>");
  client.print("<html><body>"\
                "<h1>" + String(statusCode) + " - " + String(statusText) + "</h1>"\
                "<hr>MilkyCore "__CORE_VERSION__" built "__DATE__" "__TIME__"<br/>"\
                "FreeRTOS Kernel " + tskKERNEL_VERSION_NUMBER + "</body></html>");
}

MilkyHTTPServer HTTPServer;