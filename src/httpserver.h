#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "MilkyCore.h"

#include <list>
#include <iterator>
using namespace std;

#ifndef HTTP_PORT
  #error "HTTP Port not defined. Please define HTTP_PORT"
#endif

#ifndef HTTP_MAX_CLIENT_THREADS
  #error "HTTP Client Thread Limit not defined. Please define HTTP_MAX_CLIENT_THREADS"
#endif

#ifndef HTTP_CLIENT_TIMEOUT
  #error "HTTP Client Timeout not defined. Please define HTTP_CLIENT_TIMEOUT"
#endif

class MilkyHTTPServer {
  public:
    MilkyHTTPServer(unsigned int port = HTTP_PORT) : server(port) {}
    void init();
    void restart();
    void registerUserRequestHandler(uint8_t (*handler)(HttpRequest, EthernetClient));
    void unregisterUserRequestHandler(uint8_t (*handler)(HttpRequest, EthernetClient));
    void sendHttpHeader(EthernetClient client, int statusCode, char *statusText, char *contentType);
    typedef uint8_t (*UserRequestHandler)(HttpRequest, EthernetClient);

  private:
    EthernetServer server;
    EthernetClient clients[HTTP_MAX_CLIENT_THREADS];
    HttpRequest requestHandlers[HTTP_MAX_CLIENT_THREADS];
    list<UserRequestHandler> userRequestHandlers;

    static void task(void *arg);
    static void clientHandlerTask(void *i);
    void handleHttpRequest(HttpRequest request, EthernetClient client);
    void sendDefaultStatusPage(EthernetClient client, int statusCode, char *statusText);
};

extern MilkyHTTPServer HTTPServer;

#endif