#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "Config.h"
#include <ESPAsyncWebServer.h>
#include <vector>

extern std::vector<bool> flashCodePattern;
extern AsyncWebServer server;

void setupHttpServer();

#endif // HTTPSERVER_H