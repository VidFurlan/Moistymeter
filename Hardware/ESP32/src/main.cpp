#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <iostream>

uint8_t receiverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
esp_now_peer_info_t peerInfo;

typedef struct struct_message
{
  bool master = false;
  String message = "";
} struct_message;

struct_message dataToSend;
struct_message receivedData;

bool isMaster = false;
bool hasMaster = false;

/*See if esp is master or slave - if the
desegnated pin has voltage its in master mode */
static bool master_slave(int pin)
{
  pinMode(pin, INPUT);

  if (digitalRead(pin))
  {
    Serial.println("ESP NOW set to master mode \n");
    dataToSend.master = true;
    return true;
  }

  Serial.println("ESP NOW set to slave mode \n");
  dataToSend.master = false;
  return false;
}

static void esp_now_add_peer_addr ()
{
  // Move the receaver address to the peerInfo var
  for (int i = 0; i < sizeof(receiverAddress); i++)
  {
    peerInfo.peer_addr[i] = receiverAddress[i];
  }
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer(s)");
    return;
  }
}

static void send_data(String message)
{

  dataToSend.message = message;

  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&dataToSend, sizeof(dataToSend));
}

static void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Successful" : "Failed");
}

static void on_data_receive(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
  char macStr[18];
  Serial.println("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.println(receivedData.master ? "Master: " : "Slave: ");
  Serial.println(receivedData.message);

  if (receivedData.master)
  {
    for (int i = 0; i < sizeof(receiverAddress); i++)
    {
      receiverAddress[i] = mac_addr[i];
    }

    esp_now_add_peer_addr();
    send_data("Hello sefe");
  }
}

static void esp_now_setup()
{
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Didn't initializr ESP NOW");
    return;
  }

  esp_now_register_send_cb(on_data_sent);
  esp_now_register_recv_cb(on_data_receive);

  isMaster = master_slave(4);
  if (isMaster) { esp_now_add_peer_addr(); }
}

void setup()
{
  Serial.begin(115200);

  esp_now_setup();
}

void loop()
{
  if (isMaster) { 
    send_data("Hello friend"); 
    delay(5000);
  }
}