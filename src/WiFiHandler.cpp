//
// Created by JanHe on 09.01.2026.
//

#include "WiFiHandler.h"

#include <WiFi.h>

#include "InternalConfig.h"

void WiFiHandler::setup()
{
    // Begin Soft AP.
    WiFi.softAP(NAME, PASSWORD);

    // Enable Soft AP.
    WiFi.enableAP(true);

    #if DEBUG == true
        // Print Debug Message.
        Serial.println("WiFi started");
    #endif
}

void WiFiHandler::checkConnection()
{
}
