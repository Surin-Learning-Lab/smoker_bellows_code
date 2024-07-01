// include this sketch in a seperate tab in your smokerBelows.ino sketch

#include <WebServer.h>

// Variables defined in main_sketch.ino
extern double celsius;
extern unsigned long elapsedTime;

// Web server instance
WebServer server(80);

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/temp", handleTemp);
  server.begin();
  Serial.println("HTTP server started");
}

void handleRoot() {
  char buffer[2048];
  double fahrenheit = celsius * 9.0 / 5.0 + 32.0;
  snprintf(buffer, sizeof(buffer), 
    "<!DOCTYPE html>"
    "<html lang=\"en\">"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<style>"
    "body { background-color: #875856; }"
    ".header { display: grid; justify-content: center; align-content: center; }"
    "p { font-weight: bold; }"
    ".time, .temp { display: grid; font-size: 25px; background-color: antiquewhite; "
    "padding: 20px; border-radius: 10px; }"
    "</style>"
    "<title>ESP32 Temperature and Time</title>"
    "</head>"
    "<body class=\"header\">"
    "<div>"
    "<h1>ESP32 Temperature and Time</h1>"
    "<p class=\"temp\">Temperature: %.2f &#8451;</p>"
    "<p class=\"temp\">Temperature: %.2f &#8457;</p>"
    "<p class=\"time\">Time: %02lu:%02lu</p>"
    "</div>"
    "</body>"
    "</html>", 
    celsius, fahrenheit, (elapsedTime / (60 * 60 * 1000)), (elapsedTime / (60 * 1000)) % 60);

  server.send(200, "text/html", buffer);
}

void handleTemp() {
  char buffer[64];
  double fahrenheit = celsius * 9.0 / 5.0 + 32.0;
  snprintf(buffer, sizeof(buffer), "Celsius: %.2f, Fahrenheit: %.2f", celsius, fahrenheit);
  server.send(200, "text/plain", buffer);
}

void loopWebServer() {
  server.handleClient();
}
