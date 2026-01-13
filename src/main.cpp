#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid     = "ESP32-Access-Point-Test"; // ? No clue why its char*
const char* password = "123456789"; // ? No clue why its char*

int PinLed = 2;

WebServer server(80); // ? Port to use

const char html[] PROGMEM = R"html_text(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP CONTROLLER</title>
</head>
<body>
    <div>
        <h1>ESP CONTROLLER</h1>
        <div class="button" onclick="ledOn()">Prender LED</div>
        <div class="button" onclick="ledOff()">Apagar LED</div>
        <p id="ledStatus">OFF</p>
    </div>
</body>
<script>
let ledStatus = document.getElementById('ledStatus');
function ledOn() {
    fetch('/led/on').then(response => checkLedStatus())
}
function ledOff() {
    fetch('/led/off').then(response => checkLedStatus())
}
function checkLedStatus() {
    fetch('/led/status').then(response => response.text()).then(text => ledStatus.innerHTML = text);
}
checkLedStatus();
</script>
</html>)html_text";

// • Adapted the code from https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/HelloServer/HelloServer.ino
void handleRoot() {
    server.send_P(200, "text/html", html);
};

void handleStatus() {
    bool state = digitalRead(PinLed);
    server.send(200, "text/plain", state ? "ON" : "OFF");
}

void handleNotFound() {
    digitalWrite(PinLed, 1);
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
    digitalWrite(PinLed, 0);
}

void setup(void) {
    delay(1000);
    pinMode(PinLed, OUTPUT);
    digitalWrite(PinLed, 0);
    Serial.begin(115200);
    // WiFi.mode(WIFI_STA); // ? WiFi Station (client that connects to a network)
    WiFi.mode(WIFI_AP); // ? WiFi Access Point
    // WiFi.begin(ssid, password); // ? It basically is "connect to this network with this password", doesn't create a network
    WiFi.softAP(ssid, password); // ? Sets the SSID and password of the AP

    // ? This was basically a while it's not connected, retry
    // while (WiFi.status() != WL_CONNECTED) {
    //     delay(500);
    // }

    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", handleRoot);
    server.on("/led/status", handleStatus);
    server.on("/led/on", []() {
        digitalWrite(PinLed, true);
        server.send(200, "text/plain", "led on");
    });
    server.on("/led/off", []() {
        digitalWrite(PinLed, false);
        server.send(200, "text/plain", "led off");
    });

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();
    delay(2);  // ? Allows the cpu to switch to other tasks
}