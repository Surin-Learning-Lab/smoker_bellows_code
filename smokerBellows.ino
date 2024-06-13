#include <Wire.h>
#include <max6675.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define TEMPERATURE_DISPLAY_TIME 1000
#define TIMER_DISPLAY_TIME 1000

unsigned long previousDisplayTime = 0;
unsigned long displayDuration = TEMPERATURE_DISPLAY_TIME;
bool isTemperatureDisplay = true;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo myservo;

int pos = 0;
int servoPin = 14;
const int thermoDO = 19;
const int thermoCS = 23;
const int thermoCLK = 5;
int relayPin = 12;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

bool relayOn = false;

// Multiple SSIDs and passwords
const char* ssids[] = {"Galaxy A32E945", "Surinlearninglab-2.4G"};
const char* passwords[] = {"xwtt3718", "SurinLL2023"};
const int numNetworks = sizeof(ssids) / sizeof(ssids[0]);

double celsius = 0;
unsigned long startTime;
unsigned long elapsedTime = 0;

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  connectToWiFi();

  delay(1000);
  pinMode(relayPin, OUTPUT);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50); // Set servo frequency
  myservo.attach(servoPin, 1000, 2400); // Attach servo with min and max pulse widths

  startTime = millis(); // initialize the start time

  setupWebServer();
}

void loop() {
  unsigned long currentMillis = millis();
  elapsedTime = currentMillis - startTime;

  if (currentMillis - previousDisplayTime >= displayDuration) {
    isTemperatureDisplay = !isTemperatureDisplay;
    previousDisplayTime = currentMillis;

    if (isTemperatureDisplay) {
      displayDuration = TEMPERATURE_DISPLAY_TIME;

      celsius = thermocouple.readCelsius();
      double fahrenheit = thermocouple.readFahrenheit();

      if (!isnan(celsius) && !isnan(fahrenheit)) {
        Serial.print("Celsius: ");
        Serial.print(celsius);
        Serial.print("  Fahrenheit: ");
        Serial.println(fahrenheit);

        if (celsius >= 26.50 && !relayOn) {
          digitalWrite(relayPin, HIGH);
          relayOn = true;

          // Move servo to 70 degrees
          for (pos = 0; pos <= 70; pos += 1) {
            myservo.write(pos);
            delay(5);
          }
        } else if (celsius < 26.00 && relayOn) {
          digitalWrite(relayPin, LOW);
          relayOn = false;

          // Move servo back to 0 degrees
          for (pos = 70; pos >= 0; pos -= 1) {
            myservo.write(pos);
            delay(5);
          }
        }

        display.clearDisplay();
        display.setTextSize(3);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(10, 2);
        display.print("Temp");
        display.setCursor(10, 32);
        display.print(celsius);
        display.display();
      } else {
        Serial.println("Error reading temperature. Check wiring and sensor.");
      }
    } else {
      displayDuration = TIMER_DISPLAY_TIME;

      // Timer display logic
      display.clearDisplay();
      display.setTextSize(3);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 2);
      display.print("Timer");

      // Calculate hours, minutes, and seconds
      unsigned long hours = elapsedTime / (60 * 60 * 1000);
      unsigned long minutes = (elapsedTime / (60 * 1000)) % 60;

      // Print hours, minutes, and seconds
      display.setCursor(10, 32);
      if (hours < 10) {
        display.print("0");
      }
      display.print(hours);
      display.print(":");
      if (minutes < 10) {
        display.print("0");
      }
      display.print(minutes);

      display.display();
    }
  }

  loopWebServer();
  delay(2000);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi");

  for (int i = 0; i < numNetworks; i++) {
    WiFi.begin(ssids[i], passwords[i]);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(1000);
      Serial.println(String("Connecting to ") + ssids[i] + "...");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(String("Connected to ") + ssids[i]);
      Serial.print("Local IP address: ");
      Serial.println(WiFi.localIP());
      return; // Exit the function once connected
    } else {
      Serial.println(String("Failed to connect to ") + ssids[i]);
    }
  }
  
  Serial.println("Failed to connect to any Wi-Fi network. Check credentials and try again.");
}
