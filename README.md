# ESP32 Car - Web controller

## Overview
This project is a learning-oriented ESP32 application whose final goal is to build a WiFi-controlled RC car.

The ESP32 hosts a web server that allows controlling sensors and actuators in real time through a browser interface.

## Project Goals

### Learning Goals
These goals focus on learning how to use each individual component:
- Implement a web server with multiple routes
- Read distance data from an ultrasonic sensor
- Control a servo motor
- Drive multiple DC motors

### Final Goals
- Build a 180° radar system using a servo and ultrasonic sensor
- Build a functional RC car capable of moving forward, backward and rotating

## Hardware

### Components used
- 1x ESP32 (Ch9102x Dual Core)
- 1x Ultrasonic sensor (HC-SR04)
- 1x Step-down (LM2596)
- 3x Lithium batteries (18650)
    *(Only two are required; 3 are used due to the available battery holder)*

### Planned components
- 1x Servo motor (SG92R)
- 2x Microrreduced motors (Pololu 6V 1000 rpm)
- Additional DC-DC Step-down modules (LM2596)

## Software Architecture
- The ESP32 runs an HTTP server
- Each route controls a specific hardware function
- Hardware configuration is isolated in a private `config.cpp` file
- Sensor readings are exposed through simple HTTP endpoints

## Repository Structure
├── docs/               # Schematics, wiring diagrams and documentation
├── include/            # Header files
├── lib/                # External libraries (currently unused)
├── src/
│   ├── config.cpp.example  # Configuration template
│   ├── config.cpp          # Private configuration (not tracked)
│   └── main.cpp            # Main application
├── test/               # Reserved for future tests
└── root.html           # Standalone HTML file for local web UI development

## Configuration
Before compiling, create a `config.cpp` file based on `config.cpp.example` and fill in the required credentials and configuration values.

This file is intentionally not tracked by Git.

## Notes & Limitations
- The HC-SR04 is temporarily powered at 3.3V due to the absence of a voltage divider on the ECHO pin
- Echo signal level shifting is not implemented yet

## Roadmap
- [ ] Servo-based radar
- [ ] Motor driver integration
- [ ] Web UI improvements
- [ ] JSON API responses