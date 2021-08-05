#ifndef TELNETSERVER_H
#define TELNETSERVER_H

#include "MilkyCore.h"

#include <list>
#include <iterator>
using namespace std;

#if defined(ENABLE_ETHERNET) && defined(ETHERNET_ENABLE_TELNET)
  #ifndef TELNET_PORT
    #error "Telnet Port not defined. Please define TELNET_PORT"
  #endif
  #ifndef TELNET_MAX_CLIENT_THREADS
    #error "Telnet Client Thread Limit not defined. Please define TELNET_MAX_CLIENT_THREADS"
  #endif
#endif

class MilkyTelnetServer {
  public:
    #if defined(ENABLE_ETHERNET) && defined(TELNET_MAX_CLIENT_THREADS)
      MilkyTelnetServer(unsigned int port = TELNET_PORT) : server(port) {}
      void init();
      void restart();
      void registerUserRequestHandler(uint8_t (*handler)(String, String, EthernetClient));
      void unregisterUserRequestHandler(uint8_t (*handler)(String, String, EthernetClient));
      typedef uint8_t (*UserRequestHandler)(String, String, EthernetClient);
    #endif

  private:
    #if defined(ENABLE_ETHERNET) && defined(TELNET_MAX_CLIENT_THREADS)
      EthernetServer server;
      EthernetClient clients[TELNET_MAX_CLIENT_THREADS];
      HttpRequest requestHandlers[TELNET_MAX_CLIENT_THREADS];
      list<UserRequestHandler> userRequestHandlers;

      static void task(void *arg);
      static void clientHandlerTask(void *i);
      void handleCommand(String command, EthernetClient client);
    #endif
};

extern MilkyTelnetServer TelnetServer;

#endif