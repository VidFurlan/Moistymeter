#include <WiFi.h>
#include <esp_now.h>
#include <iostream>

// ESP NOW data struct
typedef struct struct_message
{
  int id = 0; // 0 for master
  String message;
} struct_message;

struct_message myData;

// Time
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000; // Timeout time

// Sleep time
RTC_DATA_ATTR int bootCount = 0;
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 5

// Sensors GPIO pins
int photoresistorPin = 27;

// Sensors variables
float brightnes = 0.0f;

void OnDataReceive(const uint8_t* mac_addr, const uint8_t* incomingData, int len)
{
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
    mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);

  Serial.println(myData.message);

}

static void esp_now_setup()
{
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataReceive);
}

static void go_to_sleep()
{
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
    " Seconds");
  Serial.println("Going to sleep");

  delay(1000);
  Serial.flush();
  esp_deep_sleep_start();
}

void setup()
{
  esp_now_setup();

  Serial.begin(115200);

  // Boot counter
  delay(1000);
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  // Set pins
  pinMode(photoresistorPin, INPUT);
}

void loop() {
    delay(5000);  
}
/*Don't remove - if you do, the program won't run and everything will get FUCKED UP cuz the main loop tries to get executed*/