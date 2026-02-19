#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <PZEM004Tv30.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
// 🔌 PZEM UART Pins
#define PZEM_RX_PIN 23
#define PZEM_TX_PIN 22
PZEM004Tv30 pzem(Serial2, PZEM_RX_PIN, PZEM_TX_PIN);

unsigned long bootTime = 0;
bool showIP = true;
// 📶 Wi-Fi Credentials
const char* ssid = "Airtel_Yarana";
const char* password = "Abhishek@7052";
// 🌐 Servers
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
// 🪛 Sensor values
float voltage = 0, current = 0, power = 0, energy = 0, frequency = 0, pf = 0;
unsigned long lastUpdate = 0;
// 📟 LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2); // Assuming common I2C address 0x27 for 16x2 LCD
// 📊 HTML Page with WebSocket & Chart.js
String htmlPage() {
  String html = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Yarana IoT Power Monitor</title>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.9.1/chart.min.js"></script>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      background: linear-gradient(135deg, #0a0e27 0%, #1a1f3a 50%, #0f172a 100%);
      color: #e2e8f0;
      font-family: 'Inter', 'Segoe UI', sans-serif;
      min-height: 100vh;
      padding: 20px;
      overflow-x: hidden;
    }
    .header {
      text-align: center;
      padding: 20px 0 30px;
      position: relative;
    }
    .header h1 {
      font-size: clamp(1.8rem, 5vw, 3rem);
      background: linear-gradient(135deg, #60a5fa, #a78bfa, #f472b6);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      margin-bottom: 10px;
      font-weight: 800;
      letter-spacing: -1px;
    }
    .brand {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 15px;
      margin-top: 15px;
      flex-wrap: wrap;
    }
    .brand-item {
      background: rgba(96, 165, 250, 0.1);
      padding: 8px 20px;
      border-radius: 20px;
      border: 1px solid rgba(96, 165, 250, 0.3);
      font-size: 0.9rem;
      color: #60a5fa;
      backdrop-filter: blur(10px);
    }
    .status {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      background: rgba(34, 197, 94, 0.15);
      padding: 8px 16px;
      border-radius: 20px;
      border: 1px solid rgba(34, 197, 94, 0.3);
      font-size: 0.85rem;
      margin-top: 15px;
    }
    .status-dot {
      width: 8px;
      height: 8px;
      background: #22c55e;
      border-radius: 50%;
      animation: pulse 2s infinite;
    }
    @keyframes pulse {
      0%, 100% { opacity: 1; box-shadow: 0 0 8px #22c55e; }
      50% { opacity: 0.5; box-shadow: 0 0 4px #22c55e; }
    }
    .container {
      max-width: 1400px;
      margin: 0 auto;
    }
    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
      gap: 20px;
      margin-bottom: 30px;
    }
    .card {
      background: rgba(30, 41, 59, 0.6);
      backdrop-filter: blur(20px);
      border: 1px solid rgba(148, 163, 184, 0.2);
      border-radius: 20px;
      padding: 25px;
      position: relative;
      overflow: hidden;
      transition: all 0.3s ease;
    }
    .card:hover {
      transform: translateY(-5px);
      border-color: rgba(96, 165, 250, 0.5);
      box-shadow: 0 20px 40px rgba(96, 165, 250, 0.2);
    }
    .card::before {
      content: '';
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      height: 4px;
      background: linear-gradient(90deg, #60a5fa, #a78bfa);
    }
    .card-icon {
      font-size: 2rem;
      margin-bottom: 10px;
      display: block;
    }
    .card-label {
      font-size: 0.85rem;
      color: #94a3b8;
      text-transform: uppercase;
      letter-spacing: 1px;
      margin-bottom: 8px;
    }
    .card-value {
      font-size: 2.5rem;
      font-weight: 700;
      color: #f1f5f9;
      margin-bottom: 5px;
      font-variant-numeric: tabular-nums;
    }
    .card-unit {
      font-size: 1.2rem;
      color: #60a5fa;
      font-weight: 500;
    }
    .chart-container {
      background: rgba(30, 41, 59, 0.6);
      backdrop-filter: blur(20px);
      border: 1px solid rgba(148, 163, 184, 0.2);
      border-radius: 20px;
      padding: 25px;
      margin-bottom: 20px;
    }
    .chart-title {
      font-size: 1.3rem;
      color: #f1f5f9;
      margin-bottom: 20px;
      font-weight: 600;
    }
    canvas {
      max-height: 300px;
    }
    .footer {
      text-align: center;
      padding: 30px 20px;
      border-top: 1px solid rgba(148, 163, 184, 0.1);
      margin-top: 40px;
    }
    .footer-text {
      color: #64748b;
      font-size: 0.9rem;
      margin-bottom: 10px;
    }
    .creator {
      color: #60a5fa;
      font-weight: 600;
      font-size: 1rem;
    }
    @media (max-width: 768px) {
      .grid { grid-template-columns: 1fr; }
      .card-value { font-size: 2rem; }
    }
  </style>
</head>
<body>
  <div class="header">
    <h1>⚡ Real-Time Power Monitor</h1>
    <div class="brand">
      <div class="brand-item">📺 Yarana IoT Guru YouTube</div>
      <div class="brand-item">🚀 IoT Dashboard v2.0</div>
    </div>
    <div class="status">
      <span class="status-dot"></span>
      <span id="wsStatus">Connecting...</span>
    </div>
  </div>
  <div class="container">
    <div class="grid">
      <div class="card">
        <span class="card-icon">⚡</span>
        <div class="card-label">Voltage</div>
        <div class="card-value" id="voltage">--</div>
        <span class="card-unit">Volts</span>
      </div>
      <div class="card">
        <span class="card-icon">🔌</span>
        <div class="card-label">Current</div>
        <div class="card-value" id="current">--</div>
        <span class="card-unit">Ampere</span>
      </div>
      <div class="card">
        <span class="card-icon">💡</span>
        <div class="card-label">Power</div>
        <div class="card-value" id="power">--</div>
        <span class="card-unit">Watts</span>
      </div>
      <div class="card">
        <span class="card-icon">⚙️</span>
        <div class="card-label">Energy</div>
        <div class="card-value" id="energy">--</div>
        <span class="card-unit">kWh</span>
      </div>
      <div class="card">
        <span class="card-icon">📊</span>
        <div class="card-label">Frequency</div>
        <div class="card-value" id="frequency">--</div>
        <span class="card-unit">Hz</span>
      </div>
      <div class="card">
        <span class="card-icon">🎯</span>
        <div class="card-label">Power Factor</div>
        <div class="card-value" id="pf">--</div>
        <span class="card-unit">PF</span>
      </div>
    </div>
    <div class="chart-container">
      <div class="chart-title">📈 Power & Current Trends</div>
      <canvas id="powerChart"></canvas>
    </div>
    <div class="chart-container">
      <div class="chart-title">⚡ Voltage & Frequency Monitor</div>
      <canvas id="voltageChart"></canvas>
    </div>
  </div>
  <div class="footer">
    <div class="footer-text">Powered by ESP32 & PZEM-004T</div>
    <div class="creator">Created by Mr. Abhishek | Developer & IoT Expert</div>
    <div class="footer-text" style="margin-top: 10px;">© 2025 Yarana IoT Guru</div>
  </div>
  <script>
    let ws;
    const maxDataPoints = 20;
    
    // Chart data arrays
    let timeLabels = [];
    let powerData = [];
    let currentData = [];
    let voltageData = [];
    let frequencyData = [];
    // Power Chart
    const powerCtx = document.getElementById('powerChart').getContext('2d');
    const powerChart = new Chart(powerCtx, {
      type: 'line',
      data: {
        labels: timeLabels,
        datasets: [{
          label: 'Power (W)',
          data: powerData,
          borderColor: '#60a5fa',
          backgroundColor: 'rgba(96, 165, 250, 0.1)',
          tension: 0.4,
          fill: true,
          borderWidth: 2
        }, {
          label: 'Current (A)',
          data: currentData,
          borderColor: '#a78bfa',
          backgroundColor: 'rgba(167, 139, 250, 0.1)',
          tension: 0.4,
          fill: true,
          borderWidth: 2,
          yAxisID: 'y1'
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: true,
        plugins: {
          legend: { labels: { color: '#e2e8f0' } }
        },
        scales: {
          x: { 
            ticks: { color: '#94a3b8' },
            grid: { color: 'rgba(148, 163, 184, 0.1)' }
          },
          y: {
            ticks: { color: '#94a3b8' },
            grid: { color: 'rgba(148, 163, 184, 0.1)' },
            title: { display: true, text: 'Power (W)', color: '#60a5fa' }
          },
          y1: {
            position: 'right',
            ticks: { color: '#94a3b8' },
            grid: { display: false },
            title: { display: true, text: 'Current (A)', color: '#a78bfa' }
          }
        }
      }
    });
    // Voltage Chart
    const voltageCtx = document.getElementById('voltageChart').getContext('2d');
    const voltageChart = new Chart(voltageCtx, {
      type: 'line',
      data: {
        labels: timeLabels,
        datasets: [{
          label: 'Voltage (V)',
          data: voltageData,
          borderColor: '#f472b6',
          backgroundColor: 'rgba(244, 114, 182, 0.1)',
          tension: 0.4,
          fill: true,
          borderWidth: 2
        }, {
          label: 'Frequency (Hz)',
          data: frequencyData,
          borderColor: '#34d399',
          backgroundColor: 'rgba(52, 211, 153, 0.1)',
          tension: 0.4,
          fill: true,
          borderWidth: 2,
          yAxisID: 'y1'
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: true,
        plugins: {
          legend: { labels: { color: '#e2e8f0' } }
        },
        scales: {
          x: { 
            ticks: { color: '#94a3b8' },
            grid: { color: 'rgba(148, 163, 184, 0.1)' }
          },
          y: {
            ticks: { color: '#94a3b8' },
            grid: { color: 'rgba(148, 163, 184, 0.1)' },
            title: { display: true, text: 'Voltage (V)', color: '#f472b6' }
          },
          y1: {
            position: 'right',
            ticks: { color: '#94a3b8' },
            grid: { display: false },
            title: { display: true, text: 'Frequency (Hz)', color: '#34d399' }
          }
        }
      }
    });
    function connectWebSocket() {
      ws = new WebSocket('ws://' + window.location.hostname + ':81');
      
      ws.onopen = () => {
        document.getElementById('wsStatus').textContent = 'Connected';
        console.log('WebSocket Connected');
      };
      
      ws.onmessage = (event) => {
        const data = JSON.parse(event.data);
        updateDisplay(data);
        updateCharts(data);
      };
      
      ws.onclose = () => {
        document.getElementById('wsStatus').textContent = 'Disconnected';
        console.log('WebSocket Disconnected. Reconnecting...');
        setTimeout(connectWebSocket, 3000);
      };
      
      ws.onerror = (error) => {
        console.error('WebSocket Error:', error);
      };
    }
    function updateDisplay(data) {
      document.getElementById('voltage').textContent = data.voltage.toFixed(2);
      document.getElementById('current').textContent = data.current.toFixed(3);
      document.getElementById('power').textContent = data.power.toFixed(2);
      document.getElementById('energy').textContent = data.energy.toFixed(3);
      document.getElementById('frequency').textContent = data.frequency.toFixed(2);
      document.getElementById('pf').textContent = data.pf.toFixed(2);
    }
    function updateCharts(data) {
      const now = new Date();
      const timeStr = now.getHours() + ':' + 
                      String(now.getMinutes()).padStart(2, '0') + ':' + 
                      String(now.getSeconds()).padStart(2, '0');
      
      if (timeLabels.length >= maxDataPoints) {
        timeLabels.shift();
        powerData.shift();
        currentData.shift();
        voltageData.shift();
        frequencyData.shift();
      }
      
      timeLabels.push(timeStr);
      powerData.push(data.power);
      currentData.push(data.current);
      voltageData.push(data.voltage);
      frequencyData.push(data.frequency);
      
      powerChart.update('none');
      voltageChart.update('none');
    }
    connectWebSocket();
  </script>
</body>
</html>
  )=====";
  return html;
}
// WebSocket Event Handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_DISCONNECTED) {
    Serial.printf("[%u] Disconnected!\n", num);
  } else if (type == WStype_CONNECTED) {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
  }
}
// Send Data via WebSocket
void sendSensorData() {
  float v  = pzem.voltage();
  float i  = pzem.current();
  float p  = pzem.power();
  float e  = pzem.energy();
  float f  = pzem.frequency();
  float pfv = pzem.pf();
  // 🔐 SAFETY: handle NaN / invalid values
  voltage   = (isnan(v)  || isinf(v))  ? 0.0 : v;
  current   = (isnan(i)  || isinf(i))  ? 0.0 : i;
  power     = (isnan(p)  || isinf(p))  ? 0.0 : p;
  energy    = (isnan(e)  || isinf(e))  ? 0.0 : e;
  frequency = (isnan(f)  || isinf(f))  ? 0.0 : f;
  pf        = (isnan(pfv)|| isinf(pfv))? 0.0 : pfv;
  // 🖨️ DEBUG (optional but useful)
  Serial.printf(
    "PZEM → V:%.2f I:%.3f P:%.2f E:%.3f F:%.2f PF:%.2f\n",
    voltage, current, power, energy, frequency, pf
  );
  String json = "{";
  json += "\"voltage\":"   + String(voltage, 2)   + ",";
  json += "\"current\":"   + String(current, 3)   + ",";
  json += "\"power\":"     + String(power, 2)     + ",";
  json += "\"energy\":"    + String(energy, 3)    + ",";
  json += "\"frequency\":" + String(frequency, 2) + ",";
  json += "\"pf\":"        + String(pf, 2);
  json += "}";
  webSocket.broadcastTXT(json);
}
// Update LCD with readings
void updateLCD() {

  static unsigned long lastLCDUpdate = 0;

  if (millis() - lastLCDUpdate < 1000) return;  
  lastLCDUpdate = millis();

  // Show IP for 90 seconds
  if (millis() - bootTime <= 90000) {
    lcd.setCursor(0, 0);
    lcd.print("IP Address:     ");

    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    lcd.print("      ");
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("V:");
    lcd.print(voltage,1);
    lcd.print(" I:");
    lcd.print(current,2);
    lcd.print("   ");

    lcd.setCursor(0, 1);
    lcd.print("P:");
    lcd.print(power,1);
    lcd.print(" PF:");
    lcd.print(pf,2);
    lcd.print("   ");
  }
}
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}
void setup() {

  Serial.begin(115200);
  Serial.println("\n🚀 Yarana IoT Guru - Advanced Power Monitor");

  // ---------------- LCD INIT FIRST ----------------
  Wire.begin(33, 32);  // SDA=32, SCL=33
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Yarana IoT Guru");
  lcd.setCursor(0,1);
  lcd.print("Starting...");
  delay(2000);

  // ---------------- PZEM INIT ----------------
  Serial2.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
  delay(500);

  // ---------------- WIFI CONNECT ----------------
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");

  Serial.printf("📶 Connecting to: %s\n", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Wi-Fi Connected!");
  Serial.print("🌐 Dashboard: http://");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("IP Address:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());

  bootTime = millis();   // Start 90 sec timer

  // ---------------- SERVER START ----------------
  server.on("/", handleRoot);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("✅ Servers Started!");
}
void loop() {
  server.handleClient();
  webSocket.loop();
  
  // Send data and update LCD every 1 second
  if (millis() - lastUpdate >= 1000) {
    sendSensorData();
    updateLCD();
    lastUpdate = millis();
  }
}
