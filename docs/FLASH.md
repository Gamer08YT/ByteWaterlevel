# Flash

To flash a brand-new Board, please short IO9 and GND, then power on the Board and remove the Short between IO9 and GND.

Now the ESP32 C3 is in Bootloader Mode, so you can flash the filesystem and firmware.

To Flash the Board please connect an TTL-UART Converter like CH340 to RX, TX, GND and VCC.