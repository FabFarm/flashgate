#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "Config.h"
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

void setupHttpServer();

#endif // HTTPSERVER_H