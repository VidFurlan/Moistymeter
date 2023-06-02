#include <HTTPClient.h>
#include <WiFi.h>
#include <esp_now.h>
#include <iostream>

const uint8_t receiverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
esp_now_peer_info_t peerInfo;

typedef struct struct_message 
{
    String message;
} struct_message;

struct_message data_to_send;

void setup()
{
    data_to_send.message = "Hello Friend";

    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    for (int i=0; i<sizeof(receiverAddress); i++) {
        peerInfo.peer_addr[i] = receiverAddress[i];
        Serial.println(peerInfo.peer_addr[i]);
    }
    

    esp_now_init();
    esp_now_add_peer(&peerInfo);
    esp_now_send(receiverAddress, (uint8_t *) &data_to_send, sizeof(data_to_send));

}

void loop() 
{

}