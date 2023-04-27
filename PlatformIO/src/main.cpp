#include <HTTPClient.h>
#include <WiFi.h>
// #include <ArduinoJson.h>

// Network credentials
const char *ssid = "";
const char *password = "";

// Set web server port
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Time
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000; // Timeout time

// Sleep time
RTC_DATA_ATTR int bootCount = 0;
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5 


// Sensors GPIO pins
int photoresistorPin = 27;

// Sensors variables
float brightnes = 0.0f;

static void establish_connection() {
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

static void send_data() {
  WiFiClient client = server.available(); // Listen for incoming clients

  while (!client /*currentTime - previousTime <= timeoutTime*/) {
    client = server.available(); // Listen for incoming clients
    brightnes = digitalRead(photoresistorPin);
    delay(3000);
    Serial.println(client);

    if (client) { // If a new client connects
      currentTime = millis();
      previousTime = currentTime;

      Serial.println("New Client.");
      String currentLine = ""; // String for client data

      while (client.connected() && currentTime - previousTime <=
                                       timeoutTime) { // While client connected
        currentTime = millis();
        Serial.println("While loop");
        if (client.available()) { // if there's bytes to read from the client,
          char c = client.read(); // read a byte, then
          Serial.write(c);        // print it out the serial monitor
          header += c;
          if (c == '\n') { // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a
            // row. that's the end of the client HTTP request, so send a
            // response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1
              // 200 OK) and a content-type so the client knows what's coming,
              // then a blank line:
              Serial.println("Sending data");

              client.println("Connected");
              client.println(brightnes);
              client.println(bootCount);

              break;
            } else { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          } else if (c != '\r') { // if you got anything else but a carriage
                                  // return character,
            currentLine += c;     // add it to the end of the currentLine
          }
        }
      }
      // Clear the header variable
      header = "";

      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
  }
}

static void go_to_sleep() {
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                 " Seconds");
  Serial.println("Going to sleep");

  delay(1000);
  Serial.flush();
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);

  // Boot counter
  delay(1000);
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  // Set pins
  pinMode(photoresistorPin, INPUT);

  establish_connection();

  send_data();

  go_to_sleep();
}