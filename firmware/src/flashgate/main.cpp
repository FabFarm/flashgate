#include "Config.h"
#include "WifiUtils.h"
#include "HttpServer.h"

void setup()
{
  Serial.begin(9600);
  setupWifi();
  setupHttpServer();
}

void loop()
{
  // Nothing here
}
