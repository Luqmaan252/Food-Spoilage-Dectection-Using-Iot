##### FINAL YEAR PROJECT #####
# IoT Food Monitoring System (with Blynk)

## Project Description
This project is an IoT-based food monitoring system that detects food spoilage and monitors temperature and humidity using sensors. The system is connected to the internet using Blynk for real-time monitoring on a mobile app.

## Features
- Real-time temperature monitoring
- Humidity monitoring
- Food spoilage detection
- Mobile monitoring using Blynk App
- Arduino/ESP-based IoT system
- Alert notifications on threshold values

## Components Used
- Arduino Uno / ESP32
- DHT22 Sensor
- Gas Sensor (MQ series)
- Jumper wires and breadboard
- Internet connection (WiFi module if Arduino Uno is used)

## IoT Platform
- Blynk IoT App (Mobile Dashboard)
- Real-time data visualization
- Remote monitoring from anywhere

## How It Works
1. Sensors collect temperature, humidity, and gas data.
2. Microcontroller processes the data.
3. Data is sent to Blynk cloud via WiFi.
4. User monitors live data from Blynk mobile app.
5. Alerts are triggered when values exceed safe limits.

## Installation
1. Install Arduino IDE
2. Install Blynk library
3. Add WiFi credentials and Blynk Auth Token in code
4. Upload `.ino` file to board
5. Open Blynk App and connect dashboard

## Future Improvements
- AI-based spoilage prediction
- SMS/email alerts
- Cloud database storage
- Web dashboard integration

## Authors
Luqmaan Abdi
Saciid Abdi 
