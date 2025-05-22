#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// A Program Wemos D1 mini (clone) ra készült

const char* ssid = "BWIFI";
const char* password = "Bence_04#";

const int trigPin = D1;
const int echoPin = D2;

ESP8266WebServer server(80);

// Weboldal HTML (sötét dizájn, AJAX frissítés)
const char MAIN_page[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="hu">
<head>
  <meta charset="UTF-8">
  <title>Távolságmérő</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      background-color: #121212;
      color: #e0e0e0;
      font-family: 'Segoe UI', sans-serif;
      text-align: center;
      padding: 2rem;
    }
    h1 {
      color: #bb86fc;
    }
    .box {
      background: #1e1e1e;
      border-radius: 12px;
      padding: 1.5rem;
      margin: 1rem auto;
      width: 90%;
      max-width: 400px;
      box-shadow: 0 0 15px rgba(0,0,0,0.3);
    }
    .value {
      font-size: 2.5rem;
      color: #03dac6;
    }
    .label {
      font-size: 1rem;
      color: #aaaaaa;
    }
  </style>
</head>
<body>
  <h1>Ultrahangos Távolságmérés</h1>

  <div class="box">
    <div class="value" id="distance">--</div>
    <div class="label">Távolság (cm)</div>
  </div>

  <div class="box">
    <div class="value" id="duration">--</div>
    <div class="label">Raw Duration (µs)</div>
  </div>

  <script>
    setInterval(function() {
      fetch("/data")
        .then(response => response.json())
        .then(data => {
          document.getElementById("distance").textContent = data.distance.toFixed(2);
          document.getElementById("duration").textContent = data.duration;
        });
    }, 250);
  </script>
</body>
</html>

)=====";

// Mérési függvény
float distanceCM = 0.0;
long duration = 0;

void measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000); // max 30ms = ~5m
  distanceCM = duration * 0.0343 / 2.0;
}

// Weboldal és adatválaszok
void handleRoot() {
  server.send_P(200, "text/html", MAIN_page);
}

void handleData() {
  measureDistance();
  String json = "{\"duration\": " + String(duration) + ", \"distance\": " + String(distanceCM, 2) + "}";
  server.send(200, "application/json", json);
}

// Alap setup
void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Csatlakozás WiFi-hez");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Csatlakozva! IP-cím: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Webszerver elindult.");
}

void loop() {
  server.handleClient();
}

