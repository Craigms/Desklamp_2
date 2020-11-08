# Desklamp_2
Alternative Firmware for Mijia Xiaomi Desk Lamp 2 (ESP32 micro)

# Inspiration
This firmware was inspired by 
* [fvollmers](https://github.com/fvollmer/xiomi-desk-lamp-open-firmware
* [Torxgewinde](https://github.com/Torxgewinde/Desk-Lamp-Alternative-Firmware projects.

As the Xiaomi desklamp2 uses the ESP32 (single core) rather than the ESP8266 micro in original Xiaomi desklmap I ended up just writing from scratch.

# Goal
My main goal was add more dimming steps.  I found this lamp was a little too bright on the lowest settting when used as a bedside lamp.
I had no need for the Wifi/Bluetooth connectivity so haven't bothered to turn those on, but they would be easy to add with the Arduino ESP libraries.

# Features
* This firmware compiles with the Arduino IDE
* It incorperates the original 11 dimming settings and adds 6 more dimming steps to the lower end
* It allows for colour mixing (push and turn knob)
* Tapping the knob turns the lamp on and off.
* Setting is retained as long as power is connected.

# Setting up Arduino IDE
1. Add https://dl.espressif.com/dl/package_esp32_index.json to Arduino IDE preferences to get ESP32 support
2. Desklamp 2 uses the single core version of the ESP32 so you need to swap out some ESP32 files. I had some trouble with this and ended up just recompiling https://github.com/lbernstone/arduino-esp32-solo with the correct flags set to get it to work.
3. Add ESP32Encoder library in "Manage Libraries"

# Initial upload of firmware
1. Open the desk lamp. Three screws are hidden underneath the rubber. Slightly peel away the rubber and unfasten the screws.
2. Solder 3.3V USB-FTDI cable wires to the test points marked RXD, TXD and GND. Baud rate 115200, 8N1
3. Solder a switch (on flying leads) between IO0 and GND.
4. Power up.  There should be some human readable text sent over serial comms.
5. Close switch to put into programming mode.  Power cycle the lamp (may need to wait up to 30 seconds for power rails to drop before reconnecting power).
6. Compile the firmware and upload from within the Arduino IDE.
7. Open switch to put into run mode.  Power cycle the lamp (may need to wait up to 30 seconds for power rails to drop before reconnecting power).

