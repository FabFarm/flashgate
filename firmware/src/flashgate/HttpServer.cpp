#include "HttpServer.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>

AsyncWebServer server(80);
std::vector<bool> flashCodePattern;

void handleFlashPattern(AsyncWebServerRequest *request, JsonVariant &json)
{
  JsonObject jsonObj = json.as<JsonObject>();
  JsonArray flashPattern = jsonObj["flash_code_pattern"].as<JsonArray>();

  flashCodePattern.clear();

  for (bool val : flashPattern)
  {
    flashCodePattern.push_back(val);
  }

  request->send(200, "application/json", "{\"message\":\"Pattern updated\"}");
}

void setupHttpServer()
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", String(), false); });
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/styles.css", String(), false); });
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.js", String(), false); });
  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/flash_pattern", handleFlashPattern);
  server.addHandler(handler);
  server.begin();
}