#include <WiFi.h>
#include <WebServer.h>
WebServer server(80);
HardwareSerial zigbee(2);   // UART2

float temperature = 0.0;
float humidity = 0.0;
int gasLevel = 0;

/* -------------------- ZIGBEE STATUS -------------------- */
String lastZigbeeMsg = "No Data";
unsigned long lastZigbeeTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ZigBee UART (RX=16, TX=17)
  zigbee.begin(9600, SERIAL_8N1, 16, 17);
  zigbee.setTimeout(50);

  // WiFi Access Point
  WiFi.softAP("IndustryMonitor", "12345678");

  Serial.println("ESP32 AP Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Web routes
  server.on("/", handleRoot);
  server.on("/data", handleData);

  server.begin();
}
void loop() 
{
  server.handleClient();

  /* -------- ZigBee RECEIVE -------- */
  if (zigbee.available()) {
    String line = zigbee.readString();
    line.trim();

    if (line.indexOf(',') != -1) {
      Serial.println("ZigBee RX: " + line);
      lastZigbeeMsg = line;
      lastZigbeeTime = millis();
      parseZigbeeData(line);
    }
  }

  /* -------- Heartbeat -------- */
  static unsigned long hb = 0;
  if (millis() - hb > 3000) {
    hb = millis();
    Serial.println("ESP32 alive");
  }
}
/* Format: temp,humidity,gas */
void parseZigbeeData(String msg) {
  float t, h;
  int g;

  if (sscanf(msg.c_str(), "%f,%f,%d", &t, &h, &g) == 3) {
    temperature = t;
    humidity = h;
    gasLevel = g;
  }
}

void handleData() {
  bool zigbeeOnline = (millis() - lastZigbeeTime) < 5000;

  String json = "{";
  json += "\"temp\":" + String(temperature, 1) + ",";
  json += "\"hum\":" + String(humidity, 1) + ",";
  json += "\"gas\":" + String(gasLevel) + ",";
  json += "\"status\":\"" + String(zigbeeOnline ? "ONLINE" : "WAITING") + "\",";
  json += "\"last\":\"" + lastZigbeeMsg + "\"";
  json += "}";

  server.send(200, "application/json", json);
}


void handleRoot() {
  server.send(200, "text/html", webpage());
}

String webpage() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Industrial Safety</title>

<style>
body {
  margin: 0;
  font-family: Arial, sans-serif;
  background: radial-gradient(circle at top, #7b4bb7, #3b1c5a 60%, #1a0f2e);
  color: white;
  text-align: center;
}

.container {
  max-width: 420px;
  margin: auto;
  padding: 25px 15px;
}

h1 {
  font-size: 26px;
  margin-bottom: 8px;
}

.status {
  font-size: 15px;
  margin-bottom: 20px;
  letter-spacing: 1px;
}

.online { color: #2ecc71; }
.waiting { color: #f1c40f; }

.card {
  background: linear-gradient(145deg, rgba(255,255,255,0.18), rgba(255,255,255,0.05));
  border-radius: 16px;
  padding: 18px;
  margin: 14px 0;
  box-shadow: 0 0 25px rgba(180,120,255,0.25);
}

.label {
  font-size: 16px;
  opacity: 0.85;
  margin-bottom: 6px;
}

.value {
  font-size: 34px;
  font-weight: 600;
}

.footer {
  margin-top: 20px;
  font-size: 14px;
  opacity: 0.75;
}
</style>
</head>

<body>
<div class="container">

  <h1>Industrial Safety Monitor</h1>
  <div class="status waiting" id="status">ZigBee Status: WAITING</div>

  <div class="card">
    <div class="label">Temperature</div>
    <div class="value" id="temp">-- °C</div>
  </div>

  <div class="card">
    <div class="label">Humidity</div>
    <div class="value" id="hum">-- %</div>
  </div>

  <div class="card">
    <div class="label">Gas Level</div>
    <div class="value" id="gas">--</div>
  </div>

  <div class="footer">
    Last ZigBee Msg: <span id="last">--</span>
  </div>

</div>

<script>
setInterval(() => {
  fetch('/data')
    .then(r => r.json())
    .then(d => {
      document.getElementById('temp').innerHTML = d.temp + " °C";
      document.getElementById('hum').innerHTML  = d.hum + " %";
      document.getElementById('gas').innerHTML  = d.gas;
      document.getElementById('last').innerHTML = d.last;

      let status = document.getElementById('status');
      if (d.status === "ONLINE") {
        status.innerHTML = "ZigBee Status: ONLINE";
        status.className = "status online";
      } else {
        status.innerHTML = "ZigBee Status: WAITING";
        status.className = "status waiting";
      }
    });
}, 1000);
</script>

</body>
</html>
)rawliteral";
}
