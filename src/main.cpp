#include "Arduino.h"
#include "WiFiHandler.h"

void setup()
{
    WiFiHandler::setup();
}

void loop()
{
    WiFiHandler.checkConnection();
}
