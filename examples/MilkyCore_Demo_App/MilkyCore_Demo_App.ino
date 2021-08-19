#include <MilkyCore.h>

void setup() {
  //Core.earlyInit() will initialze the gpios and configuration memory. Must be called at the start of setup()
  Core.earlyInit();
  
  //Register our request handler with the webserver
  HTTPServer.registerUserRequestHandler(&httpRequestHandler);
  
  //Register our command handler with the telnet server and console
  CommandHandler.registerUserCommandHandler(&commandHandler);

  //Core.init() will start the operating system. No code will be execute after it.
  //It must be at the end of setup()
  Core.init();
}

void loop() {
  //Set Relay Q1 when Inputs I1 and I2 are set.
  GPIO.setDigital(Q1, GPIO.getDigital(I1) && GPIO.getDigital(I2));
  
  //Wait 10ms for some basic debounce
  vTaskDelay((10L * configTICK_RATE_HZ) / 1000L);
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

//Our custom command handler for use with the built in telnet server and serial console.
String commandHandler(String command) {
  //If the comm is "demo", then handle the request
  if (command.equals("demo")) {
    
    //Send a bit of text to the client
    return "Welcome to MilkyCore "__CORE_VERSION__" on the Wollmilchsau!\n";
    
  //If the action is "__help", then append our help
  } else if (command.equals("__help")) {
    return "         demo - Execute demo function\n";
  }
    
  //Return '$$NORES$$' to indicate that the request was not handeled by us.
  return "$$NORES$$";
}