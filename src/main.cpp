#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFi_config.h>

int PinLed = 2;
int PinTrig = 27;
int PinEcho = 26;
char distanceString[16];

WebServer server(80); // ? Port to use

const char html[] PROGMEM = R"html_text(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP CONTROLLER</title>
</head>
<style>
    body {
        margin: 0;
        min-height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        font-family: system-ui, sans-serif;
        background: #0f172a;
        color: #e5e7eb;
    }

    h1 {
        margin-bottom: 10px;
        letter-spacing: 2px;
    }

    .container {
        display: flex;
        gap: 10px;
        margin-top: 30px;
    }

    .button {
        padding: 15px;
        width: 160px;
        text-align: center;
        border-radius: 12px;
        cursor: pointer;
        font-weight: bold;
        user-select: none;
        transition: transform 0.1s, background 0.2s;
        background: #38bdf8;          /* azul sensor */
        color: #082f49;
    }

    .button:active {
        transform: scale(0.97);
    }

    .on {
        background: #22c55e;
        color: #052e16;
    }

    .off {
        background: #ef4444;
        color: #450a0a;
    }

    .status {
        margin-top: 10px;
        padding: 10px 20px;
        border-radius: 10px;
        background: #020617;
        font-size: 1.2em;
    }
</style>
<body>
    <h1>ESP CONTROLLER</h1>

    <div class="container">
        <div class="button on" onclick="ledOn()">LED ON</div>
        <div class="button off" onclick="ledOff()">LED OFF</div>
    </div>

    <div id="ledStatus" class="status">OFF</div>

    <div class="container">
        <div class="button" onclick="shootDistance()">Shoot HC SR-04</div>
    </div>

    <p class="status"><span id="distance">0</span> cm</p>

</body>
<script>
let ledStatus = document.getElementById('ledStatus');
let distance = document.getElementById('distance');
function ledOn() {
    fetch('/led/on').then(response => checkLedStatus())
}
function ledOff() {
    fetch('/led/off').then(response => checkLedStatus())
}
function checkLedStatus() {
    fetch('/led/status').then(response => response.text()).then(text => ledStatus.innerHTML = text);
}
function shootDistance() {
    fetch('/sonar/shoot').then(response => response.text().then(text => distance.innerHTML = text))
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

void handleSonar() {
    // ? Trigger
    digitalWrite(PinTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(PinTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(PinTrig, LOW);

    // ? Echo
    long duration = pulseIn(PinEcho, HIGH, 30000); // ? 30 ms timeout
    
    // ? Logic
    if (duration == 0) {
        server.send(504, "text/plain", "Timeout");
        return;
    }

    float distance = duration * 0.0343 / 2;
    snprintf(distanceString, sizeof(distanceString), "%.2f", distance);
    // server.send(200, "text/plain", distance);
    server.send(200, "text/plain", distanceString);
}

void setup(void) {
    delay(1000);
    pinMode(PinLed, OUTPUT);
    pinMode(PinTrig, OUTPUT);
    pinMode(PinEcho, INPUT);
    digitalWrite(PinLed, 0);
    Serial.begin(115200);
    if (AP) {
        WiFi.mode(WIFI_AP); // ? WiFi Access Point
        WiFi.softAP(SSID, PASSWORD); // ? Sets the SSID and password of the AP
    } else {
        WiFi.mode(WIFI_STA); // ? WiFi Station (client that connects to a network)
        WiFi.begin(SSID, PASSWORD); // ? It basically is "connect to this network with this password", doesn't create a network
        
        // ? While it's not connected, retry
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
        }
    }

    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", handleRoot);
    server.on("/led/status", handleStatus);
    server.on("/led/on", []() {
        digitalWrite(PinLed, true);
        server.send(200, "text/plain", "led on");
    });
    server.on("/sonar/shoot", handleSonar);
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