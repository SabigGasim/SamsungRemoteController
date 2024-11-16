# Samsung Remote Controller

This project enables remote control of Samsung air conditioners using an ESP32 microcontroller through a web interface, utilizing the `IRremoteESP8266` library solely for signal transmission. Unlike relying entirely on the library's Ir_Samsung module for both signal generation and transmission, providing greater customization and control over the signal creation process, and a room for future further support for different A.C. remotes.

## Features:

-   Supports basic control features like power toggle, temperature adjustment, mode selection, auto swing, and fan speed.
-   Built for ESP32 using the `IRremoteESP8266` library for IR communication.
-   Custom web interface to interact with the device over local Wi-Fi network.

## Installation:

1.  **Clone the repository:**
    `git clone https://github.com/SabigGasim/SamsungRemoteController.git` 
    
2.  **Install necessary libraries/packages (Same version preferred):**
    -   `IRremoteESP8266` version 2.7.18
    -   `ESPAsyncWebServer` version 3.1.0
    -   `ArduinoJson` version 7.2.0
    -   `Little_FS_esp32` version 1.0.6
    -   Tested to work on `Arduino IDE` version 2.3.3
    -   `ESP32 by Espressif Systems` version 2.0.4 in the board manager.
      
3. **Install LittleFS:**
    For more info, see: https://github.com/lorol/LITTLEFS

4.  **Upload files to ESP32:**
    On Arduino IDE, press `Ctrl + Shift + P` and choose `Upload LittleFS to Pico/ESP8266/ESP32`
5. **Configure WiFi:** Make a `Secrets.h` file in the same folder as the .ino file, and #define your `WIFI_SSID` and `WIFI_PASSWORD`

## Circuit:
**Components:**
- ESP32 (Dev module)
- USB cable (2.0 preferred)
- IR LED:
  - Voltage: 1.4v - 1.6v
  - Current: 20mA
  - Wavelength: 940nm (a must)
- 10Ω to 50Ω resistor (If connected to a USB 2.0, modify the resistance as needed based on the power supply).
- Two jumper wires (optional: F-F with a Breadboard).

**Wiring:**
The circuit powers the IR LED through the anode (longer leg, positive) via GPIO5 (D5) of the ESP32 with a 10Ω resistor to limit current. The cathode (shorter leg, negative) is connected to ground (GND).

![Circuit](https://github.com/user-attachments/assets/99b30650-b308-4a8b-8c2f-b0c5a813dd1b)



## Usage:

1.  Connect your ESP32 to the same network as your phone or computer.
2.  Access the web interface by navigating to the IP address of the ESP32 in a web browser (Predefined as `http://192.168.1.169`).
3.  Use the interface to control the Samsung AC.

## License:

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
