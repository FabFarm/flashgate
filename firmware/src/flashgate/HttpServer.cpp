#include "HttpServer.h"

AsyncWebServer server(80);

void setupHttpServer()
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Hello World"); });

  // Start server
  server.begin();
}