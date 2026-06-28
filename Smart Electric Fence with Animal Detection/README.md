# 🛡️ Smart Electric Fence and Animal Detection using ESP32

An IoT-based Smart Electric Fence system developed using ESP32 that detects unauthorized fence disturbances and immediately generates an alert. The project demonstrates real-time monitoring, embedded system programming, sensor interfacing, and security applications.

---


🎥 Project Demonstration</h2>

[Watch Demo](Smart_Electric_Fence_Demo.mp4.mp4)

# 📸 Project Images

## Block Diagram

[View Block Diagram](Images/Blockdiagram.jpeg)

## Final Prototype

[View Prototype](Images/Prototype.jpeg)

## System Work Flow

[View System Workflow](Images/Systemworkflow.jpeg)

---

# 📖 Project Overview

The Smart Electric Fence is designed to monitor a fence line continuously and detect any unauthorized disturbance. When the fence wire is interrupted or tampered with, the ESP32 processes the event and activates an alarm while displaying the event on the Serial Monitor.

The project demonstrates the practical implementation of Embedded Systems for perimeter security applications.

---

# ✨ Features

✔ Real-time fence monitoring

✔ ESP32 based controller

✔ Instant intrusion detection

✔ Buzzer alert

✔ Serial Monitor notification

✔ Low-cost implementation

✔ Expandable to IoT applications

✔ Easy to deploy

-----------------------

# Components Used

| Sl. No. | Component | Quantity |
|:-------:|-----------|:--------:|
| 1 | ESP32 Dev Board | 1 |
| 2 | ESP32-CAM Module | 1 |
| 3 | PIR Motion Sensor (HC-SR501) | 2 |
| 4 | Ultrasonic Sensor (HC-SR04) | 2 |
| 5 | 2-Channel Relay Module | 1 |
| 6 | High-Power LED / Flashlight | 1 |
| 7 | Breadboard & Jumper Wires | 1 Set |
| 8 | 12V Adapter / Battery Pack | 1 |
| 9 | Miscellaneous Components (Resistors, PCB, Enclosure, Connectors) | As Required |

------------------------------

## ⚙️ Working Principle

- The PIR sensors continuously monitor the protected area for motion.
- When motion is detected, the ultrasonic sensors verify the object's presence and distance.
- The ESP32 processes the sensor data and triggers the ESP32-CAM to capture an image.
- The captured image can be classified using an AI model to determine whether the object is an animal or a human.
- If an animal is detected, the ESP32 activates the relay, which powers the electric fence and turns on a high-power LED to deter intrusion.
- The system uploads event details and captured images to the cloud via Wi-Fi for remote monitoring.
- Once the event is handled, the system resets automatically and resumes continuous surveillance.

---

# 📊 Block Diagram

[View Block Diagram](Images/Blockdiagram.jpeg)

# 🚀 How to Run

1. Install Arduino IDE.

2. Install ESP32 Board Package.

3. Open SmartFence.ino.

4. Select the correct COM Port.

5. Upload the code.

6. Open Serial Monitor.

7. Trigger the fence.

8. Observe the alert.

---

# 📈 Applications

• Farm Protection

• Industrial Security

• Warehouse Monitoring

• Military Perimeter Security

• Residential Security

• Restricted Area Monitoring

---

# 🔮 Future Improvements

- GSM SMS Alert

- Wi-Fi Notification

- Mobile App

- Firebase Integration

- Cloud Dashboard

- Solar Powered System

- Battery Backup

- CCTV Integration

- AI-based Intrusion Detection

---

# 💻 Technologies Used

- Embedded C

- Arduino IDE

- ESP32

- IoT

- Embedded Systems

- Electronics

---

# 👨‍💻 Author

Madhusudhan C

Electronics and Communication Engineering

BMS College of Engineering

---

# ⭐ If you like this project

Please consider giving this repository a ⭐ on GitHub.
