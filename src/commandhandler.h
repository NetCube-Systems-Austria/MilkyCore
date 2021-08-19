#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include "MilkyCore.h"

#include <list>
#include <iterator>
using namespace std;

class MilkyCommandHandler {
  public:
    void registerUserCommandHandler(String (*handler)(String));
    void unregisterUserCommandHandler(String (*handler)(String));
    String handleCommand(String command);
    typedef String (*UserCommandHandler)(String);

  private:
    list<UserCommandHandler> userCommandHandlers;
};

extern MilkyCommandHandler CommandHandler;

#endif