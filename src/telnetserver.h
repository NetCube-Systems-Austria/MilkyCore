#ifndef TELNETSERVER_H
#define TELNETSERVER_H

#include "MilkyCore.h"

#include <list>
#include <iterator>
using namespace std;

#ifndef TELNET_PORT
  #error "Telnet Port not defined. Please define TELNET_PORT"
#endif

#ifndef TELNET_MAX_CLIENT_THREADS
  #error "Telnet Client Thread Limit not defined. Please define TELNET_MAX_CLIENT_THREADS"
#endif

class MilkyTelnetServer {
  public:
    MilkyTelnetServer(unsigned int port = TELNET_PORT) : server(port) {}
    void init();
    void restart();

  private:
    EthernetServer server;
    EthernetClient clients[TELNET_MAX_CLIENT_THREADS];
    HttpRequest requestHandlers[TELNET_MAX_CLIENT_THREADS];

    static void task(void *arg);
    static void clientHandlerTask(void *i);
    void handleCommand(String command, EthernetClient client);
};

extern MilkyTelnetServer TelnetServer;

#endif