#include <Arduino.h>
#include <esp32-setup-lib.h>

void setup()
{
    Serial.begin(115200);
    pressESCtoEnterSetup(4000, 500);
}

void loop() 
{

}