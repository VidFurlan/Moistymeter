#include <WiFi.h>
#include <esp_now.h>
#include <iostream>

//MAC Addr for the client
const uint8_t receiverAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
esp_now_peer_info_t peerInfo;

typedef struct struct_message
{
  int id = 0; // 0 for master
  String message;
} struct_message;

struct_message data_to_send;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Successful" : "Delivery Failed");
}

static void esp_now_setup()
{
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Didn't initializr ESP NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Move the receaver address to the peer info var
  for (int i = 0; i < sizeof(receiverAddress); i++) {
    peerInfo.peer_addr[i] = receiverAddress[i];
    Serial.println(peerInfo.peer_addr[i]);
  }
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peers");
    return;
  }
}

void setup()
{
  data_to_send.message = "Hello Friends! (Sent from server)";

  Serial.begin(115200);

  esp_now_setup();
}

void loop()
{
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t*)&data_to_send, sizeof(data_to_send));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(5000);
}