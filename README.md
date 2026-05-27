# 🤖 Smart File Delivery Robot

An autonomous line-following robot that delivers files between rooms using IoT technology.

![Status](https://img.shields.io/badge/Status-Completed-success)
![Platform](https://img.shields.io/badge/Platform-Arduino%20%7C%20ESP8266-blue)
![Firebase](https://img.shields.io/badge/Cloud-Firebase-orange)
![License](https://img.shields.io/badge/License-MIT-green)

---

## ✨ Features

- 🛤️ **Autonomous Navigation** - 5-channel IR sensor array for precise line following
- 🔀 **Junction Detection** - Handles T-junctions and L-turns automatically
- 📡 **IoT Control** - Real-time control via Firebase web dashboard
- 🚧 **Obstacle Avoidance** - Ultrasonic sensor with auto pause/resume
- 📺 **Live Display** - LCD shows current status and location
- 📦 **Auto Delivery** - Servo-controlled box opens on arrival
- 🌐 **Web Dashboard** - Modern UI for mission control

---

## 🏗️ System Architecture

```
┌─────────────────┐         ┌─────────────────┐
│   Web Dashboard │◄───────►│    Firebase     │
│   (HTML/CSS/JS) │   WiFi  │ Realtime Database│
└─────────────────┘         └────────┬────────┘
                                     │
                                     ▼
                            ┌─────────────────┐
                            │  NodeMCU ESP8266 │
                            │  • WiFi/Firebase │
                            │  • Ultrasonic    │
                            │  • LCD Display   │
                            │  • Servo Motor   │
                            └────────┬────────┘
                                     │ Serial (9600 baud)
                                     ▼
                            ┌─────────────────┐
                            │   Arduino Uno   │
                            │  • 5 IR Sensors │
                            │  • 4 DC Motors  │
                            │  • Line Following│
                            └─────────────────┘
```

---

## 📍 Track Layout

```
                    Room C
                      │
                      │
    Room A ─────────[J1]───────── Room B
                      │
                      │
                 Base Station
```

---

## 🔧 Hardware Components

| Component | Qty | Purpose |
|-----------|-----|---------|
| Arduino Uno | 1 | Motor control & line following |
| NodeMCU ESP8266 | 1 | WiFi & Firebase communication |
| 5-Channel IR Sensor | 1 | Line detection |
| HC-SR04 Ultrasonic | 1 | Obstacle detection |
| L298N Motor Driver | 2 | DC motor control |
| DC Motors + Mecanum Wheels | 4 | Movement |
| 16x2 I2C LCD | 1 | Status display |
| SG90 Servo | 1 | Box opening |
| Buzzer | 1 | Alerts |

---

## 📌 Pin Configuration

### Arduino Uno

| Pin | Component |
|-----|-----------|
| A0-A4 | IR Sensors (Right to Left) |
| 12, 11 | Front Left Motor |
| 10, 8 | Front Right Motor |
| 7, 4 | Back Right Motor |
| 2, 13 | Back Left Motor |
| 3, 5, 6, 9 | PWM Speed Control |

### NodeMCU ESP8266

| Pin | Component |
|-----|-----------|
| D1 | LCD SCL |
| D2 | LCD SDA |
| D3 | Ultrasonic TRIG |
| D4 | Ultrasonic ECHO |
| D5 | Buzzer |
| D6 | Servo |
| TX/RX | Arduino Serial |

---

## 🚀 Installation

### 1. Clone the Repository
```bash
git clone https://github.com/yathindu/ROBODELIVER--smart-file-delivery-robot--.git
```

### 2. Upload Arduino Code
- Open `Arduino/Arduino_Code.ino` in Arduino IDE
- Select Board: **Arduino Uno**
- Upload

### 3. Upload NodeMCU Code
- Open `NodeMCU/NodeMCU_Code.ino`
- Update WiFi & Firebase credentials
- Select Board: **NodeMCU 1.0 (ESP-12E Module)**
- Upload

### 4. Launch Dashboard
- Open `Dashboard/index.html` in browser
- Update Firebase config
- Start controlling!

---

## 🔥 Firebase Structure

```
robots/
└── robot1/
    ├── command: "START_MISSION" | "RETURN_HOME" | "NONE"
    ├── queue: "Room A"
    ├── status: "IDLE" | "MOVING" | "WAITING"
    ├── currentTarget: "Room A"
    ├── arrivedAt: "Room A"
    └── obstacleDetected: true | false
```

---

## 📡 Communication Protocol

| Direction | Command | Action |
|-----------|---------|--------|
| NodeMCU → Arduino | `GO` | Start moving |
| NodeMCU → Arduino | `STOP` | Stop motors |
| NodeMCU → Arduino | `PAUSE` | Pause for obstacle |
| NodeMCU → Arduino | `RESUME` | Resume movement |
| NodeMCU → Arduino | `TURN:LEFT` | Turn left |
| NodeMCU → Arduino | `TURN:RIGHT` | Turn right |
| NodeMCU → Arduino | `TURN:STRAIGHT` | Go straight |
| Arduino → NodeMCU | `JUNCTION` | Junction detected |
| Arduino → NodeMCU | `ARRIVED` | Destination reached |

---

## 🌐 Web Dashboard

| Page | Description |
|------|-------------|
| `index.html` | Home - System overview |
| `control.html` | Control - Mission control panel |
| `monitor.html` | Monitor - Live sensor data |
| `logs.html` | Logs - Event history |

---

## 👥 Contributors

| Member | Contribution |
|--------|--------------|
| [Yathindu] | Line Following Algorithm, Turn Execution, Dashboard(Index page)|
| [Chanuka] | Motor Control Functions, Dashboard(Logs page) |
| [Sindupa] | WiFi, Firebase, Obstacle Detection, Dashboard(Monitor)|
| [Pasindu] | Navigation Routes, LCD Display, Dashboard(Control page) |

---

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.

---
