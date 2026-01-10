//
// Created by JanHe on 09.01.2026.
//

#include "WiFiHandler.h"

#include <WiFi.h>

#include "InternalConfig.h"

// I use an UniFi Network on 192.XXX.XXX.XXX so i will use an 10.XX.XX.XX for Debugging.
IPAddress apIP(10, 10, 10, 1);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);

void WiFiHandler::setup()
{
    // Set AP mode explicitly
    WiFi.mode(WIFI_AP);

    // Define Subnet before beginning Soft AP.
    WiFi.softAPConfig(apIP, gateway, subnet);

    // Begin Soft AP.
    WiFi.softAP(NAME, PASSWORD);
#if DEBUG == true
    // Print Debug Message.
    Serial.println("WiFi started");
#endif

    Serial.print("Ready on");
    Serial.println(WiFi.softAPIP());
}

void WiFiHandler::loop()
{
    checkConnection();
}

void WiFiHandler::checkConnection()
{
}
