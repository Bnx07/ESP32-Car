#include <Arduino.h>
#include <WiFi.h>

const char* ssid     = "ESP32-Access-Point-Test"; // ? No clue why its char*
const char* password = "123456789"; // ? No clue why its char*

int PinLed = 2;

// ! Proceed with https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer

void WiFiEvent(WiFiEvent_t event) {
    // ? https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientEvents/WiFiClientEvents.ino
    // ? Events 12 and 14: Occur in new connection
    // ? Event 13: Occurs in disconnection
    if (event == 12) { // ? New connection
        digitalWrite(PinLed, true);
    } else if (event == 13) { // ? Disconnected
        digitalWrite(PinLed, false);
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(PinLed, OUTPUT);
    digitalWrite(PinLed, false);

    WiFi.onEvent(WiFiEvent); // ? Sets the function WifiEvent whenever an event occurs

    Serial.println("Setting AP (Access Point)...");
    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
}

void loop() {
}