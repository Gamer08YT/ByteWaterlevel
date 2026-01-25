# ByteWaterlevel

ByteWaterlevel in Short (BYTELEVEL) is an Device to Monitor Wells, Cisterns and co. 
It uses an TL136 Sensor to monitor the Fluid Level.

It comes with two Relais to control Pumps, Sirens or other Appliances.

The Device is designed only to work with external Antennas to provide a good Wi-Fi Range.

<img height="600px" width="auto" src="./assets/img/bytelevel.png"/>


## Features:
- HTTP API
  - Basic Auth
  - Enable/Disable Relais
  - Enable Relais for a given Time period
  - Restart Device
  - Save Configuration
  - Get Device Info (CPU Temperature, Sensor etc...)
- MQTT
- WiFi
  - AP Mode
  - Client Mode
- USB
  - Flashing
  - JTAG
- GUI
  - Status
  - Configuration
  - Info

## Specifications:

* Input: 12-24V (with Reverse protection)
* USB: Micro USB (with Overvoltage protection)
* MCU: ESP32 C3
* Relais current (peak): **max 10A** 
* Power consumption: low
* Sensor: TL136 (Optional with Overvoltage protection)

## Setup

For Setup please have a Look into <a href="./SETUP.md">SETUP.md</a>.

## Factory reset:

For Factory reset have a Look into <a href="./RESET.md">RESET.md</a>.

## API Docs

For API Docs please have a Look into <a href="./API.md">API.md</a>.

## Used Software

- esp32async/ESPAsyncWebServer
- esp32async/AsyncTCP
- anchon/ArduinoJson
- bertmelis/espMqttClient

## Screenshots

### Status
![img.png](assets/img/status.png)

### Configuration
![img_1.png](assets/img/configuration.png)

### System
![img_2.png](assets/img/system.png)

## Disclaimer
**As this is a prototype, we accept no liability for damage to persons or buildings.**

**Any warranty is excluded.**

**The device should be installed by a qualified electrician in accordance with VDE regulations.**
