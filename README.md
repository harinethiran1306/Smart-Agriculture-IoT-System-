# 🌱 Smart Agriculture IoT System

## 📌 Overview

The Smart Agriculture IoT System is an automated monitoring and irrigation solution designed to support efficient and sustainable agricultural practices. The system continuously monitors key environmental parameters such as soil moisture, temperature, humidity, and water availability, and uses this data to automate irrigation while providing real-time feedback to users.

An ESP32-based Maker Feather AIoT S3 microcontroller serves as the core controller, integrating multiple sensors and actuators. Sensor data is transmitted to the V-One IoT Platform, enabling real-time monitoring, alerts, and remote control through a cloud-based interface.

---

## ✨ Features

- **Automated Irrigation**  
  Controls watering based on real-time soil moisture levels to ensure optimal irrigation.

- **Environmental Monitoring**  
  Tracks temperature, humidity, and soil moisture to support healthy crop growth.

- **Remote Control**  
  Allows manual control and automation through the V-One IoT dashboard.

- **Alerts & Notifications**  
  Generates alerts when soil moisture falls below predefined thresholds.

- **Failsafe Mechanism**  
  Detects low or empty water tank conditions and activates a buzzer alert to prevent irrigation failure.

---

## 🧰 Hardware Components

- ESP32 microcontroller (Maker Feather AIoT S3)
- DHT11 temperature and humidity sensor
- Soil moisture sensor
- Non-contact liquid level sensor
- Servo motor (irrigation valve control)
- Buzzer (low water level alert)
- Wi-Fi connectivity

---

## 📁 Folder Structure

**Smart Agriculture IoT System Report.pdf**     # Project report

**arduino-code/**  
└── **CPC357_Project_Arduino_Code.ino**        # ESP32 firmware (main logic)

**vone-workflow-code/**  
└── **moisture.py**                            # V-One workflow: soil moisture automation and alert logic

---

## ⚙️ How It Works

### ESP32 Firmware (**arduino-code/CPC357_Project_Arduino_Code.ino**)

- Reads sensor data (temperature, humidity, soil moisture, and water level).
- Publishes telemetry data to the V-One IoT Platform.
- Receives actuator commands for manual servo motor control.
- Automatically controls irrigation based on soil moisture levels.
- Activates the buzzer when the water tank level is critically low.

### V-One Workflow Script (**vone-workflow-code/**)

- **moisture.py**  
  Monitors soil moisture levels and triggers irrigation or alerts when values fall below predefined thresholds.

---

## 🚀 Getting Started

### 1. Hardware Setup

- Connect all sensors and actuators to the ESP32 according to the pin configuration defined in **CPC357_Project_Arduino_Code.ino**.
- Ensure Wi-Fi credentials and V-One device IDs are correctly configured.

### 2. Firmware Setup

- Open **CPC357_Project_Arduino_Code.ino** using Arduino IDE.
- Install the required libraries:
  - **ESP32Servo**
  - **DHT**
  - **V-One MQTT client**
- Upload the firmware to the ESP32 microcontroller.

### 3. V-One IoT Platform Setup

- Register the following devices on the V-One platform:
  - Soil Moisture Sensor
  - Servo Motor
  - DHT11 Sensor
- Configure the soil moisture workflow using **moisture.py**.
- Adjust threshold values in the script based on agricultural requirements.

---

## 📄 Project Report

For detailed system design, implementation, testing, and results, refer to:

**Smart Agriculture IoT System Report.pdf**

---

## 📜 License

This project is developed for educational purposes only.  
Please refer to the project report for references and acknowledgements.

---

## 👤 Authors

Contributor details are available in the project report.

---

## 📚 References

- **V-One IoT Platform**
- **Arduino & ESP32 Documentation**
- **DHT11 Sensor Documentation**
