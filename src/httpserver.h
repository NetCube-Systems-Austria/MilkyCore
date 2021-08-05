#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "MilkyCore.h"

#if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_HTTP)
  #ifndef HTTP_PORT
    #error "HTTP Port not defined. Please define HTTP_PORT"
  #endif
  #ifndef HTTP_MAX_CLIENT_THREADS
    #error "HTTP Client Thread Limit not defined. Please define HTTP_MAX_CLIENT_THREADS"
  #endif
  #ifndef HTTP_CLIENT_TIMEOUT
    #error "HTTP Client Timeout not defined. Please define HTTP_CLIENT_TIMEOUT"
  #endif
#endif

class MilkyHTTPServer {
  public:
    #if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_HTTP)
      MilkyHTTPServer(unsigned int port = HTTP_PORT) : server(port) {}
      void init();
      void restart();
    #endif

  private:
    #if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_HTTP)
      EthernetServer server;
      EthernetClient clients[HTTP_MAX_CLIENT_THREADS];
      HttpRequest requestHandlers[HTTP_MAX_CLIENT_THREADS];

      static void task(void *arg);
      static void clientHandlerTask(void *i);
      void handleHttpRequest(HttpRequest request, EthernetClient client);
      void sendHttpHeader(EthernetClient client, int statusCode, char *statusText, char *contentType);
      void sendDefaultStatusPage(EthernetClient client, int statusCode, char *statusText);
    #endif
};

extern MilkyHTTPServer HTTPServer;

#endif