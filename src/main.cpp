#include "Arduino.h"
#include "WebHandler.h"
#include "WiFiHandler.h"

void setup()
{
    // Begin Serial.
    Serial.begin(115200);

    // Setup WiFi Connection.
    WiFiHandler::setup();

    // Setup Web.
    WebHandler::setup();
}

void loop()
{
    // Check for WiFi Connection.
    WiFiHandler::checkConnection();

    // Handle Web.
    WebHandler::loop();
}
