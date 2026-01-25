//
// Created by JanHe on 25.01.2026.
//

#include <Arduino.h>
#include <Arduino_ESP32_OTA.h>
#include "OTAHandler.h"
#include "FileHandler.h"

void OTAHandler::setup()
{
    if (FileHandler::getConfig()["ota"].as<bool>())
    {


#if DEBUG == true
        Serial.println("OTA started");
#endif
    }
}
