// Draadloze communicatie proof of concept: WiFi access point + webpagina
// Board: Arduino Nano ESP32
// Verbind je laptop of smartphone met WiFi "LineFollower" (wachtwoord "linefollower"),
// open daarna http://192.168.4.1 en druk op de knop.

#include <WiFi.h>
#include <WebServer.h>

const char* AP_SSID = "LineFollower";
const char* AP_PASS = "linefollower";   // minimaal 8 tekens

WebServer server(80);
unsigned long pingCount = 0;

void handleRoot() {
  server.send(200, "text/html",
    "<html><body style='font-family:sans-serif'>"
    "<h1>Line follower</h1>"
    "<button onclick=\"fetch('/ping').then(r=>r.text()).then(t=>document.getElementById('out').textContent=t)\">Ping</button>"
    "<p id='out'>-</p></body></html>");
}

void handlePing() {
  pingCount++;
  Serial.print("Ping ontvangen: ");
  Serial.println(pingCount);
  server.send(200, "text/plain", "pong " + String(pingCount));
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("WiFi aan, open http://");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/ping", handlePing);
  server.begin();
}

void loop() {
  server.handleClient();
}
