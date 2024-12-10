# Samsung Remote Controller

## Description:

This project enables remote control of Samsung air conditioners using an ESP32 microcontroller through a web interface, utilizing the `IRremoteESP8266` library solely for signal transmission. Unlike relying entirely on the library's Ir_Samsung module for both signal generation and transmission, this approach provides greater customization and control over the signal creation process, unlocking potential for further support of various A.C. remote controllers.

## Features:

-   Supports basic control features like power toggle, temperature adjustment, mode selection, auto swing, and fan speed.
-   Implements translation from A.C. values to both Samsung's 14-byte and extended 21-byte protocols.
-   Built for ESP32 using the `IRremoteESP8266` library for IR communication.
-   Custom web interface to interact with the device over local Wi-Fi network.


    ![20241118_000019](https://github.com/user-attachments/assets/ccf19d61-27e4-46a7-84b5-2bcb0466dfef)

## Prerequisites
Install [PlatformIO IDE](https://platformio.org/) for your environment:
 - **Option 1**: Install as an extension in [Visual Studio Code](https://code.visualstudio.com/).
 - **Option 2**: Use the [PlatformIO Core CLI](https://docs.platformio.org/en/latest/core/index.html) if you prefer working from the command line.

## Installation:

1.  **Clone the repository:**
    `git clone https://github.com/SabigGasim/SamsungRemoteController.git` 
    
2.  **Install necessary libraries (Same version preferred):**
    -   `IRremoteESP8266` version 2.8.6
    -   `ArduinoJson` version 7.2.0
    -   `ESPAsyncWebServer by khoih-prog` should automatically get installed once you compile the project.
      
3. **Install LittleFS:**
    For more info, refer to: https://github.com/lorol/LITTLEFS

4.  **Upload files to ESP32:**
    On PlatformIO terminal, run `pio run --target uploadfs`
5. **Configure WiFi:** Make a `Secrets.h` file under SamsungRemoteController/Include/, then #define your `WIFI_SSID` and `WIFI_PASSWORD`

## Circuit:
**Components:**
- ESP32 (Dev module)
- USB cable (2.0 preferred)
- IR LED:
  - Voltage: 1.4v - 1.6v
  - Current: 20mA
  - Wavelength: 940nm (a must)
- 10Ω to 50Ω resistor (Connected to a USB 2.0. Modify the resistance as needed based on the power supply).
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