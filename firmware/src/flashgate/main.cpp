#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "fabfarm_ele_container";
const char* password = "imakestuff";

// Create an instance of the server
// Specify the port to listen on as an argument
AsyncWebServer server(80);

void setup() {
  // Serial port for debugging purposes
  Serial.begin(9600);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello World");
  });

  // Start server
  server.begin();
}

void loop() {
  // Nothing here
}
