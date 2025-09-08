# Design HMI of Cluster Display by Qt Creator

## Overview.
This is a Qt Quick-based instrument cluster application designed for embedded automotive systems. It features a graphical dashboard with real-time vehicle data, CAN bus communication, and a modern UI. The project is structured for easy integration with Yocto-based embedded Linux systems. The project is structured for easy integration with Yocto-based embedded Linux systems

![Diagram](../My_HMI.png)

## Key Features

- **Real-Time Dashboard:** Displays speed, battery level, distance traveled, and warning indicators.
- **CAN Bus Integration:** Receives and processes vehicle data through SocketCAN for reliable in-vehicle communication.
- **Qt Quick / QML UI:** Modern, customizable user interface optimized for embedded screens.
- **Yocto Integration:** Easily built and deployed within Yocto-based embedded Linux distributions.
- **Scalable Architecture:** Structured for modular development, supporting future expansion of features.

## Directory Structure
```bash
.
├── assets
│   ├── background.png
│   ├── background.svg
│   ├── ........
├── BatteryGauge.qml
├── CAN_Communication
│   ├── CanHandler.cpp
│   └── CanHandler.h
├── Cluster.pro
├── Cluster.pro.user
├── main.cpp
├── main.qml
└── qml.qrc

```
