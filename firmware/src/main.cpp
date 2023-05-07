#include <Arduino.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

// Pins
constexpr int lightSensorPin = 34;
constexpr int gateRelayPin = 4;

// MQTT
const char *mqtt_server = "your_mqtt_server";
const int mqtt_port = 1883;
const char *mqtt_user = "your_mqtt_user";
const char *mqtt_pass = "your_mqtt_password";
const char *gate_topic = "homeassistant/gate";
const char *gate_config_topic = "homeassistant/gate/config";

// Globals
String highBeamPattern = "1010"; // Default high beam pattern
constexpr unsigned long debounceDelay = 200; // Debounce delay in milliseconds
int gateTimeout = 10000; // Gate timeout in milliseconds

// Objects
WiFiClient espClient;
PubSubClient mqttClient(espClient);
AsyncWebServer server(80);

// Function prototypes
void openGate();
void closeGate();
bool isHighBeamPattern(const String &pattern);
void mqttCallback(char *topic, byte *payload, unsigned int length);
void mqttReconnect();
void serveStaticFiles();

void setup() {
  Serial.begin(115200);
  pinMode(lightSensorPin, INPUT);
  pinMode(gateRelayPin, OUTPUT);
  digitalWrite(gateRelayPin, LOW);

  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  // Initialize WiFi Manager
  WiFiManager wifiManager;
  wifiManager.autoConnect("ESP32_Gate_AP");

  // Initialize MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  // Initialize and start the web server
  serveStaticFiles();
  server.begin();
}

void loop() {
  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.loop();

  // Read light sensor value
  int lightSensorValue = analogRead(lightSensorPin);

  // Check for high beam
  bool highBeam = lightSensorValue > 2000;

  // Implement pattern detection and debounce logic
  static String pattern = "";
  static unsigned long lastDebounceTime = 0;
  static bool lastHighBeamState = false;
  unsigned long currentTime = millis();

  if (highBeam != lastHighBeamState) {
    lastDebounceTime = currentTime;
  }

  if (currentTime - lastDebounceTime > debounceDelay) {
    pattern += highBeam ? '1' : '0';
    lastHighBeamState = highBeam;
  }

  // Trim the pattern string to the length of the expected high beam pattern
  if (pattern.length() > highBeamPattern.length()) {
    pattern.remove(0, 1);
  }

  // Check if the detected pattern matches the expected high beam pattern
  if (isHighBeamPattern(pattern)) {
    openGate();
    pattern = ""; // Reset the pattern
  }

  delay(100); // Add a short delay to prevent excessive CPU usage
}

void openGate() {
  digitalWrite(gateRelayPin, HIGH);
  mqttClient.publish(gate_topic, "open");
  delay(gateTimeout);
  closeGate();
}

void closeGate() {
  digitalWrite(gateRelayPin, LOW);
  mqttClient.publish(gate_topic, "close");
}

bool isHighBeamPattern(const String &pattern) {
  return pattern == highBeamPattern;
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  String incomingPayload;
  incomingPayload.reserve(length);
  for (int i = 0; i < length; i++) {
    incomingPayload += (char)payload[i];
  }

  if (String(topic) == gate_config_topic) {
    // Parse incoming JSON and update settings
    StaticJsonDocument<200> doc;
    deserializeJson(doc, incomingPayload);

    if (doc.containsKey("highBeamPattern")) {
      highBeamPattern = doc["highBeamPattern"].as<String>();
    }
    if (doc.containsKey("gateTimeout")) {
      gateTimeout = doc["gateTimeout"].as<int>();
    }
  }
}

void mqttReconnect() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32_Gate", mqtt_user, mqtt_pass)) {
      mqttClient.subscribe(gate_config_topic);
    } else {
      delay(5000);
    }
  }
}

void serveStaticFiles() {
  // Serve the HTML file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Serve the CSS file
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/styles.css", "text/css");
  });

  // Serve the JavaScript file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script.js", "application/javascript");
  });
}
