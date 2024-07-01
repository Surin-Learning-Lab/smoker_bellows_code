ESP32 Temperature Control System
Welcome to the ESP32 Temperature Control System project! This project utilizes an ESP32 microcontroller to monitor and control the temperature within a homemade smoker, using a thermocouple for precise temperature measurement and a servo to adjust air flow. The setup also features an OLED display for real-time status updates and a relay for additional control options. The project is highly versatile and can be adapted for other temperature-dependent applications.

Features
Precise Temperature Monitoring: Uses a MAX6675 thermocouple module to accurately measure temperatures.
Dynamic Control: Automatically adjusts a servo and a relay based on the temperature readings to maintain the desired conditions.
Real-Time Feedback: Displays current temperature and system status on an SSD1306 OLED display.
Web Interface: Includes a customizable HTML interface to monitor and control the system remotely.
Prerequisites
Before you start, make sure you have the following:

ESP32 development board
MAX6675 thermocouple module
Servo motor
Relay module
SSD1306 OLED display
Basic electronics tools (soldering iron, wires, breadboard)
Arduino IDE with ESP32 board definitions installed
Hardware Setup
Connect the Thermocouple:

DO to GPIO 19
CS to GPIO 23
CLK to GPIO 5
Setup the Servo:

Control wire to GPIO 14
Setup the Relay:

Control wire to GPIO 12
Connect the OLED Display:

SDA and SCL to the corresponding pins on the ESP32
Ensure all connections are secure and double-check your wiring before powering the device.

Software Setup
Install Required Libraries:

Adafruit GFX Library
Adafruit SSD1306 Library
MAX6675 Library

Load the sketch into Arduino IDE.
Adjust the network settings and pin configurations as necessary.
Upload the Code:

Connect the ESP32 via USB.
Select the correct board and port.
Upload the sketch.
Usage
Once the code is uploaded and the system is running:

The OLED display will show the current temperature and system status.
Access the web interface from any device on the same network to monitor or adjust settings.
Customizing
Web Interface: Modify the HTML and CSS in the web server files to match your specific needs or preferences.
Control Logic: Adjust the temperature thresholds and control parameters in the sketch according to your application requirements.
Contributing
Contributions to this project are welcome! Feel free to fork the repository and submit pull requests. You can also open issues for bugs or feature requests.

License
This project is licensed under the MIT License - see the LICENSE file for details.

Acknowledgements
Thanks to the Arduino and ESP32 communities for their comprehensive resources and libraries.
