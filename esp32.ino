#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);
DHT dht(4, DHT22);

const int fanL = 18, fanR = 19, light = 21;
bool isManual = false;
unsigned long manualTime = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(fanL, OUTPUT); pinMode(fanR, OUTPUT); pinMode(light, OUTPUT);
  WiFi.begin(ssid, password);
  
  server.on("/", []() {
    isManual = true; manualTime = millis();
    digitalWrite(light, HIGH); // Example manual action
    server.send(200, "text/html", "Manual Mode Active for 30m");
  });
  server.begin();
}

void loop() {
  server.handleClient();
  
  // Timeout: revert to auto after 30 mins
  if (isManual && (millis() - manualTime > 1800000)) isManual = false;

  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    if (!isManual) {
      int l = data.substring(data.indexOf('L')+1, data.indexOf('R')).toInt();
      int r = data.substring(data.indexOf('R')+1).toInt();
      float t = dht.readTemperature();
      
      digitalWrite(fanL, (l > 0 && t > 25) ? HIGH : LOW);
      digitalWrite(fanR, (r > 0 && t > 25) ? HIGH : LOW);
      digitalWrite(light, (l + r > 0) ? HIGH : LOW);
    }
  }
}