# ⚡ ESP32 Smart Energy Meter  
### Real-Time Power Monitor with WebSocket Dashboard  
By **Yarana IoT Guru**

---

## 🚀 Project Overview

This project is a **Real-Time IoT Energy Monitoring System** built using:

- ESP32 WiFi Microcontroller  
- PZEM-004T v3.0 Energy Monitoring Module  
- Embedded Web Server  
- WebSocket Real-Time Communication  
- Chart.js Live Dashboard  
- 16x2 I2C LCD Display  

The ESP32 reads electrical parameters from the PZEM-004T sensor and streams live data to a responsive web dashboard without page refresh using WebSocket.

---

## 🔥 Features

✔ Real-Time Voltage Monitoring  
✔ Real-Time Current Monitoring  
✔ Live Power (Watt) Tracking  
✔ Energy (kWh) Monitoring  
✔ Frequency Display  
✔ Power Factor Monitoring  
✔ WebSocket-Based Live Dashboard (No Refresh)  
✔ Dual Live Charts (Power/Current & Voltage/Frequency)  
✔ Local LCD Display Output  
✔ Embedded HTML Dashboard inside ESP32 firmware  

---

## 📊 Parameters Monitored

| Parameter | Unit |
|------------|------|
| Voltage | Volts (V) |
| Current | Ampere (A) |
| Power | Watts (W) |
| Energy | kWh |
| Frequency | Hz |
| Power Factor | PF |

---

## 🧰 Hardware Requirements

- ESP32 Development Board  
- PZEM-004T v3.0 Energy Monitor  
- 16x2 I2C LCD (Address 0x27)  
- 5V Power Supply  
- CT Coil (Install on AC Phase Wire Only)

⚠ **Electrical Safety Warning:**  
This project involves AC mains. Always follow proper insulation and safety precautions.

---

## 🔌 Wiring Connections

### ESP32 ↔ PZEM-004T

| ESP32 Pin | PZEM Pin |
|------------|----------|
| GPIO23 | RX |
| GPIO22 | TX |
| 5V | VCC |
| GND | GND |

---

### ESP32 ↔ I2C LCD

| ESP32 | LCD |
|--------|------|
| GPIO32 | SDA |
| GPIO33 | SCL |
| 5V | VCC |
| GND | GND |

---

## 🌐 How It Works

1. ESP32 connects to WiFi.
2. PZEM-004T reads electrical parameters.
3. Invalid (NaN / Inf) values are filtered.
4. Data is formatted into JSON.
5. JSON is sent via WebSocket.
6. Dashboard updates instantly without refresh.
7. LCD initially shows IP for 90 seconds.
8. After that, LCD displays real-time power data.

---

## 💻 Dashboard Technology

- Embedded HTML inside `.ino` file
- WebSocket (Port 81)
- Chart.js for Live Graphs
- Responsive Modern UI
- Dark Theme Professional Interface

Access Dashboard at:

```
http://ESP32_IP_ADDRESS
```

---

## 🧠 Libraries Used

Install the following libraries in Arduino IDE:

- WiFi.h
- WebServer.h
- WebSocketsServer.h
- PZEM004Tv30.h
- LiquidCrystal_I2C.h
- Wire.h

---

## 📂 Repository Structure

```
esp32-smart-energy-meter/
│
├── esp32_smart_energy_meter.ino
│
└── README.md
```

Note: The full dashboard HTML is embedded inside the `.ino` file.

---

## 🏠 Applications

- Smart Home Energy Monitoring  
- Office Electricity Tracking  
- Industrial Power Analysis  
- Energy Efficiency Research  
- Embedded Systems Training  
- Final Year IoT Projects  

---

## 🎥 Full Video Tutorial

Watch Complete Step-by-Step Tutorial:

👉 https://youtu.be/THy6PcdtL04

---

## 🚀 Future Enhancements

- MQTT Cloud Integration
- Energy Bill Estimation
- Data Logging to Database
- Overload Detection with Relay
- Mobile App Version
- OTA Firmware Update

---

## 💼 About Yarana IoT Guru

Yarana IoT Guru focuses on building practical IoT solutions using:

- ESP32
- MQTT
- WebSocket
- Embedded Web Servers
- Automation Systems
- Industrial IoT Architectures

---

## 📞 Contact

📩 Email: contact@yaranaiotguru.in  
📲 WhatsApp: +91 70527 22734  
🌐 Website: https://yaranaiotguru.in  
📁 GitHub: https://github.com/YaranaIotGuru  

---

⭐ If you found this project useful, please consider giving this repository a star.
