/*
http://randomnerdtutorials.com  
https://randomnerdtutorials.com/esp32-http-get-post-arduino/
https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
*/

#include <WiFi.h>
#include <HTTPClient.h>
//#include <ArduinoJson.h>

// Network credentials
const char* ssid = "VidPixel";
const char* password = "12345678";

// Set web server port
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Time
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000; // Timeout time

//Sleep time
RTC_DATA_ATTR int bootCount = 0;
#define uS_TO_S_FACTOR 50000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  15        /* Time ESP32 will go to sleep (in seconds) */

// Sensors GPIO pins
int photoresistorPin = 27;

// Sensors variables
float brightnes = 0.0f;

static void send_data()
{
  
  while (currentTime - previousTime <= timeoutTime) {
    WiFiClient client = server.available();   // Listen for incoming clients

    brightnes = digitalRead(photoresistorPin);
    delay(3000);
    Serial.println(client);        

    if (client) { // If a new client connects
      currentTime = millis();
      previousTime = currentTime;

      Serial.println("New Client.");        
      String currentLine = ""; // String for client data

      while (client.connected() && currentTime - previousTime <= timeoutTime) {  // While client connected
        currentTime = millis();
        Serial.println("While loop");
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          header += c;
          if (c == '\n') {                    // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              Serial.println("Sending data");

              client.println("Connected");
              client.println(brightnes);
              client.println(bootCount);

              break;
            } else { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
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

static void go_to_sleep ()
{
  //print_wakeup_reason();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");
    Serial.println("Going to sleep");
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
}
/* static void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}*/

void setup() {
  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  // Set pins
  pinMode(photoresistorPin, INPUT);

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

  delay(2000);

  send_data();

  go_to_sleep();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  
  brightnes = digitalRead(photoresistorPin);
  Serial.println(brightnes);

  if (client) { // If a new client connects
    currentTime = millis();
    previousTime = currentTime;

    Serial.println("New Client.");        
    String currentLine = ""; // String for client data

    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // While client connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            Serial.println("Sending data");

            client.println("Connected");
            client.println(brightnes);
            client.println(bootCount);

            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
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