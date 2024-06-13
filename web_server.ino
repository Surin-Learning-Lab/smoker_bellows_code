#include <WebServer.h>

// Use the same external arrays for SSIDs and passwords
extern const char* ssids[];
extern const char* passwords[];
extern double celsius;
extern unsigned long elapsedTime;

WebServer server(80);

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/temp", handleTemp);
  server.begin();
  Serial.println("HTTP server started");
}

void handleRoot() {
  char buffer[2048];
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
    "<p class=\"time\">Time: %02lu:%02lu</p>"
    "</div>"
    "</body>"
    "</html>", 
    celsius, (elapsedTime / (60 * 60 * 1000)), (elapsedTime / (60 * 1000)) % 60);

  server.send(200, "text/html", buffer);
}

void handleTemp() {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%.2f", celsius);
  server.send(200, "text/plain", buffer);
}

void loopWebServer() {
  server.handleClient();
}
