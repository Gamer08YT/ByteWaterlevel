# Setup

## Firmware Setup

If you received a blank ESP32 on your board, or you damaged the firmware, please look at <a href="./FLASH.md">FLASH.md</a>.

## Initial Setup

When powering up the Device for the first Time, it opens a Wi-Fi AP, you need then to connect to the Wi-Fi.

#### Default Credentials:

Password: **BYTESTORE2026**

![img.png](assets/img/wifi-pc.png)

After connecting to the Device, open a Webbrowser on your System and open http://10.10.10.1 to open the WebUI.

![img_1.png](assets/img/configuration.png)

Fill in your Wi-Fi Credentials you want to connect to and press "Save", the Device will now reboot and connect to your
Wi-Fi Network.

## Installation

### Basic Wiring

Please only use SELV Voltage, you can control big Pumps, for example, with external Contactors.

Work should always be carried out in accordance with local guidelines, e.g. VDE. We accept no liability for personal injury, property damage, or other damage.

![schematic.png](assets/img/schematic.png)

Please note that the relay channels are only designed for 10A for short periods; external contactors should be used for continuous loads.
