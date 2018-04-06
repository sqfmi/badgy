## Setup
* Add ESP8266 Arduino Core using the Boards Manager https://github.com/esp8266/Arduino#installing-with-boards-manager
* Install the following libraries
- GxEPD library https://github.com/ZinggJM/GxEPD
- WiFi Manager library https://github.com/tzapu/WiFiManager

## Updating The Firmware
1. To update the firmware, press the center button 5 times. This will turn on the WiFi and Badgy's IP address will be shown
2. Go to *Sktech* -> *Export compiled Binary* to create your firmware binary *.bin
3. Go to http://*BADGY_IP_ADDRESS*/update to upload your new *.bin file
4. Badgy will now restart with the new firmware
### Note: If your new firmware does not include the WiFi and OTA code, you will no longer be able to update over WiFi!

## Troubleshoot
* If for some reason your code is crashing and OTA updates isn't working, you can manually flash the firmware using the pads on the PCB. Using a USB-Serial adapter (e.g. FTDI), connect the pins to the pad:
```
+------------+-----------+
| USB-Serial | Badgy Pad |
+------------+-----------+
| 3.3V       | 3.3V      |
| GND        | GND       |
| TX         | RX        |
| RX         | TX        |
| GND        | 0         |
+------------+-----------+
```
