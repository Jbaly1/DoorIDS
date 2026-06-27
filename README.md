# DoorIDS
A lightweight distributed intrusion detection system built using an ESP32, Raspberry Pi, and MQTT.

DoorIDS detects when a door is opened, closed, or held open using a magnetic reed switch. Events are transmitted from an ESP32 over a private TCP network to a Raspberry Pi, which publishes them to an MQTT broker. An Android device running Termux subscribes to the MQTT topic and displays real-time notifications.

The project was built as a personal embedded systems project to explore low-level programming, networking, Linux services, and IoT communication.

# Features
- Detects door open events
- Detects door closed events
- Detects doors left open for an extended period
- Non-blocking firmware using software timers
- Automatic Wi-Fi reconnection
- Automatic TCP reconnection
- Raspberry Pi TCP server
- MQTT message publishing
- Android notifications through Termux
- Lightweight event protocol

# System Architecture
Door\
 │\
 ▼\
Magnetic Reed Switch\
 │\
 ▼\
ESP32\
 │\
 │ TCP\
 ▼\
Raspberry Pi\
 │\
 │ MQTT\
 ▼\
Android Phone (Termux)\
 │\
 ▼\
Notification

# Hardware
- ESP32 Development Board
- Magnetic Reed Switch
- Passive Buzzer
- Raspberry Pi
- Android phone running Termux

# Software
# ESP32
- Arduino Framework
- Wi-Fi Client
- TCP Socket Client
# Raspberry Pi
- Python
- TCP Socket Server
- Eclipse Mosquitto MQTT Broker
- Paho MQTT
# Android
- Python
- Paho MQTT
- Termux API

# Communication Protocol

The ESP32 transmits single-character event codes to minimise bandwidth and simplify parsing.

Code & Meaning\
O	= Door Open\
C	= Door Closed\
H	= Door Held Open

The Raspberry Pi converts these into MQTT messages before forwarding them to subscribed devices.

# Repository Structure
DoorIDS/\
│\
├── firmware/\
│   └── esp32/\
│\
├── server/\
│\
├── phone/\
│\
├── docs/\
│\
├── images/\
│\
└── README.md

# Current Limitations
- Firmware currently uses the Arduino framework
- Communication is not yet encrypted
- Supports a single monitored door
- MQTT authentication is still being refined
- Android notifications currently rely on Termux

# Future Improvements
- Port firmware to ESP-IDF
- Use FreeRTOS tasks
- Add TLS encryption
- Multiple door support
- Tamper detection
- Battery backup
- OTA firmware updates
- Web dashboard
- Event logging
- Camera integration
- Home Assistant support

# Why I Built This

I wanted to build a complete embedded system rather than a standalone microcontroller project.

DoorIDS combines embedded programming, Linux administration, networking, sockets, MQTT messaging, and mobile notifications into a single distributed system. The goal was to gain practical experience designing software that runs across multiple devices while keeping the firmware lightweight and responsive.

Although the project is still evolving, it demonstrates the core concepts used in many real-world IoT and embedded security systems.

# License

This project is released under the MIT License.
