# ByteWaterlevel

ByteWaterlevel in Short (BYTELEVEL) is an Device to Monitor Wells, Cisterns and co. 
It uses an TL136 Sensor to monitor the Fluid Level.

It comes with two Relais to control Pumps, Sirens or other Appliances.

The Device is designed only to work with external Antennas to provide a good Wi-Fi Range.

<img height="600px" width="auto" src="./assets/img/bytelevel.png"/>


## Features:
- HTTP API
  - Enable/Disable Relais
  - Enable Relais for a given Time period
- MQTT
- WiFi
  - AP Mode
  - Client Mode
- USB
  - Flashing
  - JTAG

## Specifications:

* Input: 12-24V (with Reverse protection)
* USB: Micro USB (with Overvoltage protection)
* MCU: ESP32 C3
* Relais current (peak): **max 10A** 
* Power consumption: low
* Sensor: TL136 (Optional with Overvoltage protection)

## API Docs

For API Docs please have a Look into <a href="./API.md">API.md</a>.