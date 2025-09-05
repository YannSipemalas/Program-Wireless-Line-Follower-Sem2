#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Pin motor sama seperti sebelumnya
const int pwmL = 5, pwmR = 4, rotL = 0, rotR = 2;
ESP8266WebServer server(80);

// HTML page
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP Robot Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { text-align:center; font-family: Arial; }
    button { width:80px; height:40px; margin:5px; }
  </style>
</head>
<body>
  <h3>Kontrol Motor DC</h3>
  <button onclick="send('/control?left=200&right=200')">Maju</button><br>
  <button onclick="send('/control?left=-200&right=200')">Kiri</button>
  <button onclick="send('/control?left=0&right=0')">Stop</button>
  <button onclick="send('/control?left=200&right=-200')">Kanan</button><br>
  <button onclick="send('/control?left=-200&right=-200')">Mundur</button>
  <script>
    function send(path) {
      fetch(path);
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  WiFi.softAP("ESP-Robot", "12345678");
  Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());

  pinMode(pwmL, OUTPUT); pinMode(pwmR, OUTPUT);
  pinMode(rotL, OUTPUT); pinMode(rotR, OUTPUT);

  server.on("/", []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });
  server.on("/control", HTTP_GET, []() {
    int l = server.arg("left").toInt();
    int r = server.arg("right").toInt();
    digitalWrite(rotL, l >= 0);
    analogWrite(pwmL, abs(l));
    digitalWrite(rotR, r >= 0);
    analogWrite(pwmR, abs(r));
    server.send(200, "text/plain", "OK");
  });
  server.begin();
}

void loop() {
  server.handleClient();
}