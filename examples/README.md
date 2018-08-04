## Setup
- Install Arduino IDE 1.8.5 https://www.arduino.cc/en/Main/Software
- Add ESP8266 Arduino Core using the Boards Manager https://github.com/esp8266/Arduino#installing-with-boards-manager
- Install the following libraries for the various examples
    - Adafruit GFX library https://github.com/adafruit/Adafruit-GFX-Library
    - GxEPD library https://github.com/ZinggJM/GxEPD
    - ArduinoJSON library https://github.com/bblanchon/ArduinoJson
    - WiFi Manager library https://github.com/tzapu/WiFiManager
- When compiling the sketch in the Arduino IDE, choose **Tools** > **Board** > **NodeMCU 1.0 (ESP-12E Module)**    

## Uploading Your Own Firmware
1. In the Arduino IDE, go to *Sktech* -> *Export compiled Binary* to create your firmware binary `*.bin`
2. Slide the power switch off (up), hold the center button and slide the power switch on (down) at the same time. Your Badgy should now be in OTA mode.
3. Go to http://*BADGY_IP_ADDRESS*:8888/update to upload your new `*.bin` file
4. Badgy will now restart with your new firmware
### Note: If your new firmware does not include the WiFi and OTA code, you will no longer be able to update over WiFi!

## Restoring To Default Firmware
1. Download the default firmware [badgy.bin](https://github.com/sqfmi/badgy/raw/master/examples/badgy.bin)
2. Slide the power switch off (up), hold the center button and slide the power switch on (down) at the same time. Your Badgy should now be in OTA mode.
3. Go to http://*BADGY_IP_ADDRESS*:8888/update and upload `badgy.bin`
4. Badgy will now restart with the default firmware

## Manual Flashing / Recovery
* If for some reason your code is crashing and OTA updates isn't working, or if you prefer uploading over serial, you can manually flash the firmware using the programming pads on the PCB. Using a USB-Serial adapter (e.g. FTDI), connect the pins to the pads like so:
```
WARNING: ALWAYS DISCONNECT OTHER POWER SOURCES (E.G. USB/BATTERY) BEFORE CONNECTING SERIAL ADAPTER TO THE PROGRAMMING PADS.
WARNING: AVOID CONDUCTIVE CONTACT (E.G. METALS/LIQUIDS) ON THE EXPOSED PADS TO PREVENT DAMAGE
+------------+-----------+
| USB-Serial | Badgy     |
+------------+-----------+
| RX        =>    TX     |
| TX        =>    RX     |
| GND       =>    0      |
| GND       =>    GND    |
| 3V3       =>    3V3    |
| NC        =>    RST    |
+------------+-----------+
```
The blue LED on Badgy should start blinking during upload. You can use the Arduino IDE or [esptool](https://github.com/espressif/esptool) to flash your firmware.
