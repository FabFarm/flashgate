# ESP32 Gate Opener

This repository contains the firmware and web interface for an ESP32-based gate opener system. The system allows you to remotely open and close a gate using a pattern of high beam flashes from a car. The firmware detects the flashing pattern using a light sensor and unlocks the gate accordingly. The project also includes an HTML front-end with customization options and a programming mode.

## Features

- Customizable high beam pattern recognition
- MQTT integration with Home Assistant
- Remote configuration via a web interface
- Auto-connect to Wi-Fi using WiFiManager
- SPIFFS for storing web files (HTML, CSS, and JavaScript)

## Prerequisites

Before you begin, ensure you have met the following requirements:

- Install the [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)
- Install the following libraries:
  - [WiFiManager](https://github.com/tzapu/WiFiManager)
  - [PubSubClient](https://github.com/knolleary/pubsubclient)
  - [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
  - [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
  - [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)

## Installation

1. Clone this repository to your local machine.
2. Open the `gate_opener.ino` file in the Arduino IDE or PlatformIO.
3. Modify the MQTT settings in the `gate_opener.ino` file:
   ```
   const char *mqtt_server = "your_mqtt_server";
   const int mqtt_port = 1883;
   const char *mqtt_user = "your_mqtt_user";
   const char *mqtt_pass = "your_mqtt_password";
   ```
4. Compile and upload the firmware to your ESP32 board.
5. Upload the web files (HTML, CSS, and JavaScript) to the SPIFFS partition on the ESP32.

## Usage

1. Power on the ESP32 board.
2. Connect to the "ESP32_Gate_AP" Wi-Fi network, and configure the Wi-Fi settings using the captive portal.
3. Access the web interface by navigating to the ESP32's IP address in your browser.
4. Customize the high beam pattern and gate timeout settings as desired.
5. Integrate the gate opener with your Home Assistant installation using MQTT.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.