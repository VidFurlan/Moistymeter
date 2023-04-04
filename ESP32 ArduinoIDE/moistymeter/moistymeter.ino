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

// Sensors GPIO pins
int photoresistorPin = 27;

// Sensors variables
float brightnes = 0.0f;

void setup() {
  Serial.begin(115200);

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
            client.println("Connected");
            client.println(brightnes);



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
