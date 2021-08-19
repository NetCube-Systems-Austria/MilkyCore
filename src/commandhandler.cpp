#include "commandhandler.h"

void MilkyCommandHandler::registerUserCommandHandler(String (*handler)(String)) {
  userCommandHandlers.push_back(handler);
}

void MilkyCommandHandler::unregisterUserCommandHandler(String (*handler)(String)) {
  userCommandHandlers.remove(handler);
}

String MilkyCommandHandler::handleCommand(String command) {
  String result = "";
  
  if (command.equals("")) {
    return "";
  } else if (command.equals("help")) {
    result += "\n";
    result += "===============================\n";
    result += "==      MilkyCore  Help      ==\n";
    result += "===============================\n";

    for ( list<UserCommandHandler>::iterator handler = userCommandHandlers.begin(); handler != userCommandHandlers.end(); handler++ ) {
      result += (*handler)("__help");
    }

    return result;
  } else {
    for ( list<UserCommandHandler>::iterator handler = userCommandHandlers.begin(); handler != userCommandHandlers.end(); handler++ ) {
      result = (*handler)(command);
      if (!result.equals("$$NORES$$")) return result;
    }

    return "Unknown command. Type 'help' for a list of available commands.\n";
  }
}

MilkyCommandHandler CommandHandler;