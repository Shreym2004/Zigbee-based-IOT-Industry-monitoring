# ZigBee-Based Industry Monitoring System

An IoT-based industrial safety monitoring system that detects temperature, humidity, and gas levels using Arduino with ZigBee wireless communication and an ESP32 web dashboard.

## 🎯 Features

- **Real-time Monitoring**: Temperature, humidity, and gas level detection
- **Wireless Communication**: ZigBee-based data transmission between Arduino and ESP32
- **Web Dashboard**: Live monitoring interface accessible via WiFi
- **Automatic Alerts**: LED indicators and buzzer alerts for gas and temperature threshold breaches
- **Smart Ventilation**: Servo-controlled vent that opens automatically when hazards are detected
- **Status Indicators**: 
  - 🟢 Green LED: Safe conditions
  - 🟡 Yellow LED: Gas alert
  - 🔴 Red LED: Critical condition (gas + high temperature)

## 📋 Hardware Requirements

### Arduino Side (TX - Transmitter)
- Arduino UNO/Nano
- DHT11 Temperature & Humidity Sensor
- MQ-2 Gas Sensor
- XBee S1 Module (ZigBee)
- SG90 Servo Motor
- LEDs (Green, Yellow, Red)
- Buzzer
- Fan Module (relay controlled)
- Resistors and connecting wires

### ESP32 Side (RX - Receiver)
- ESP32 Development Board
- XBee S1 Module (ZigBee)
- USB cable for power

## 🔌 Pin Configuration

### Arduino (TX Side)
| Component | Pin |
|-----------|-----|
| DHT11 | Pin 4 |
| MQ-2 Gas Sensor | A0 |
| Green LED | Pin 6 |
| Yellow LED | Pin 7 |
| Red LED | Pin 8 |
| Buzzer | Pin 9 |
| Fan | Pin 10 |
| Servo | Pin 11 |
| XBee RX | Pin 2 |
| XBee TX | Pin 3 |

### ESP32 (RX Side)
| Component | GPIO |
|-----------|------|
| ZigBee RX | GPIO 16 |
| ZigBee TX | GPIO 17 |
| UART2 | Hardware Serial |

## 🚀 Setup Instructions

### Arduino Setup
1. Install the Arduino IDE from [arduino.cc](https://www.arduino.cc)
2. Install required libraries:
   - DHT sensor library by Adafruit
   - Servo library (usually pre-installed)
3. Configure XBee modules to work together at 9600 baud rate
4. Upload `Arduino_TX.ino` to your Arduino board
5. Open Serial Monitor (9600 baud) to verify: "ARDUINO READY"

### ESP32 Setup
1. Install ESP32 board support in Arduino IDE
2. Install required libraries:
   - WiFi (built-in)
   - WebServer (built-in)
3. Upload `ESP32_RX.ino` to your ESP32 board
4. ESP32 will start as WiFi Access Point (AP mode)

### Configuration
- **WiFi SSID**: `IndustryMonitor`
- **WiFi Password**: `12345678`
- **Gas Threshold**: 400 (configurable in code)
- **Temperature Threshold**: 45°C (configurable in code)
- **Baud Rate**: 9600 bps (both devices)

## 📱 Usage

1. Power both Arduino and ESP32 devices
2. Connect to WiFi network `IndustryMonitor` using password `12345678`
3. Open web browser and go to `192.168.4.1`
4. Monitor real-time sensor data on the dashboard
5. LEDs and buzzer will activate based on thresholds:
   - **Normal**: Green LED on
   - **Gas Alert**: Yellow LED + 3 buzzer beeps + Fan on + Vent opens
   - **Critical**: Red LED + Continuous buzzer + Fan on + Vent opens

## 📊 Data Format

Data is transmitted from Arduino to ESP32 via ZigBee in the format:
