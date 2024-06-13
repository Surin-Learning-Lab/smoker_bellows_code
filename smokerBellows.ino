#include <Wire.h>
#include <max6675.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
int led = 26;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

bool relayOn = false;
bool ledOn = false;

const char *ssid = "Surinlearninglab-2.4G";
const char *password = "SurinLL2023";
const char *serverAddress = "192.168.1.24";
const int serverPort = 80;

unsigned long startTime;
unsigned long elapsedTime = 0;

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  connectToWiFi();

  delay(1000);
  pinMode(relayPin, OUTPUT);
  pinMode(led, OUTPUT);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 1000, 2400);
  startTime = millis(); // initialize the start time
}

void loop() {
  unsigned long currentMillis = millis();
  elapsedTime = currentMillis - startTime;

  if (currentMillis - previousDisplayTime >= displayDuration) {
    isTemperatureDisplay = !isTemperatureDisplay;
    previousDisplayTime = currentMillis;

    if (isTemperatureDisplay) {
      displayDuration = TEMPERATURE_DISPLAY_TIME;

      double celsius = thermocouple.readCelsius();
      double fahrenheit = thermocouple.readFahrenheit();

      if (!isnan(celsius) && !isnan(fahrenheit)) {
        Serial.print("Celsius: ");
        Serial.print(celsius);
        Serial.print("  Fahrenheit: ");
        Serial.println(fahrenheit);

        if (celsius >= 26.50 && !relayOn) {
          digitalWrite(relayPin, HIGH);
          relayOn = true;
          digitalWrite(led, HIGH);
          ledOn = true;

          
          for (pos = 0; pos <= 70; pos += 1){
            myservo.write(pos);
            delay(5);
          }
        } else if (celsius < 26.00 && relayOn) {
          digitalWrite(relayPin, LOW);
          relayOn = false;
          digitalWrite(led, LOW);
          ledOn = false;
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

        sendDataToServer(celsius);
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

  // Your existing delay(2000)
  delay(2000);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
    Serial.print("Local IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi. Check credentials and try again.");
  }
}

void sendDataToServer(double temperature) {
  WiFiClient client;

  if (client.connect(serverAddress, serverPort)) {
    Serial.println("Connected to the server");

    char url[100];
    snprintf(url, sizeof(url), "/update?temp=%.2f", temperature);

    Serial.println("HTTP Request:");
    Serial.print("GET ");
    Serial.print(url);
    Serial.println(" HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(serverAddress);
    Serial.println("Connection: close\r\n\r\n");

    client.print("GET ");
    client.print(url);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(serverAddress);
    client.print("\r\n");
    client.print("Connection: close\r\n\r\n");

    Serial.println("Server Response:");
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
    }

    client.stop();
    Serial.println("Connection closed");
  } else {
    Serial.println("Unable to connect to the server");
  }
}

