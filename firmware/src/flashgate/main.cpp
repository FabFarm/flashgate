#include "Config.h"
#include "WifiUtils.h"
#include "HttpServer.h"
#include "FileSystem.h"

void setup()
{
  Serial.begin(9600);
  setupFileSystem();
  setupWifi();
  setupHttpServer();
}

void loop()
{
  // Nothing here
}
