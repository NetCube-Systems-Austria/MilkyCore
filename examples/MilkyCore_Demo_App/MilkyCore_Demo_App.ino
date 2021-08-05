#include <MilkyCore.h>

void setup() {
  //Register our request handler with the webserver
  HTTPServer.registerUserRequestHandler(&httpRequestHandler);
  
  //Register our request handler with the telnet server
  TelnetServer.registerUserRequestHandler(&telnetRequestHandler);

  //Core.init() will start the operating system. No code will be execute after it.
  //It must be at the end of setup()
  Core.init();
}

void loop() {
  SystemLogger.writeLine(SystemRTC.getCurrentTimeStr());
  vTaskDelay((7500L * configTICK_RATE_HZ) / 1000L);
}

//Our custom request handler for use with the built in webserver.
uint8_t httpRequestHandler(HttpRequest request, EthernetClient client) {
  //If the request uri is /demo.cgi, then handle the request
  if (strcmp(request.uri, "/demo.cgi") == 0) {
    //Send the header
    HTTPServer.sendHttpHeader(client, 200, "OK", "text/html");
    
    //Send the content
    client.println("<!DOCTYPE HTML>");
    client.print("<html><body>Welcome to MilkyCore "__CORE_VERSION__" on the Wollmilchsau!</body></html>");
    
    //Return 0 to indicate that we handeled the request.
    return 0;
  }
  
  //Return 1 to indicate that the request was not handeled by us.
  return 1;
}

//Our custom request handler for use with the built in telnet server.
uint8_t telnetRequestHandler(String action, String args, EthernetClient client) {
  //If the action is "demo", then handle the request
  if (action.equals("demo")) {
    
    //Send a bit of text to the client
    client.println("Welcome to MilkyCore "__CORE_VERSION__" on the Wollmilchsau!");
    
    //Return 0 to indicate that we handeled the request.
    return 0;
    
  //If the action is "help", then print our help
  } else if (action.equals("help")) {
    
    client.println("");
    client.println("===============================");
    client.println("==   Demo Application Help   ==");
    client.println("===============================");
    client.println("       demo - Execute demo function");
    client.println("");
    
    //Return 0 to indicate that we handeled the request.
    return 0;
  }
    
  //Return 1 to indicate that the request was not handeled by us.
  return 1;
}
