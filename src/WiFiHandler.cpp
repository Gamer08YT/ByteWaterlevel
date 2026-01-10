//
// Created by JanHe on 09.01.2026.
//

#include "WiFiHandler.h"
#include <WiFi.h>
#include "FileHandler.h"
#include "InternalConfig.h"

// I use an UniFi Network on 192.XXX.XXX.XXX so i will use an 10.XX.XX.XX for Debugging.
IPAddress apIP(10, 10, 10, 1);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);

void WiFiHandler::setup()
{
    // Enable Auto Reconnect.
    WiFi.setAutoConnect(true);

    JsonDocument config = FileHandler::getConfig();

    // Check if Wi-Fi Credentials are set.
    if (isWiFiClientUsable())
    {
        // Set Device Hostname (steal from AP Settings).
        WiFi.hostname(config["wifi"]["ap"]["ssid"].as<String>());

        // Set AP mode explicitly
        WiFi.mode(WIFI_MODE_STA);

        // Begin Wi-Fi Connection.
        WiFi.begin(config["wifi"]["client"]["ssid"].as<String>(), config["wifi"]["client"]["password"].as<String>());
    }
    else
    {
        // Set AP mode explicitly
        WiFi.mode(WIFI_MODE_AP);

        // Define Subnet before beginning Soft AP.
        WiFi.softAPConfig(apIP, gateway, subnet);

        // Begin Soft AP.
        WiFi.softAP(config["wifi"]["ap"]["ssid"].as<String>(), config["wifi"]["ap"]["password"].as<String>());
    }

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

/**
 * Checks if the Wi-Fi client configuration is valid and usable.
 * The method verifies whether the necessary Wi-Fi credentials
 * (SSID and password) are present in the configuration file.
 *
 * @return true if the Wi-Fi configuration contains valid credentials;
 *         false otherwise.
 */
bool WiFiHandler::isWiFiClientUsable()
{
    JsonDocument config = FileHandler::getConfig();

    return (!config["wifi"].isNull() && !config["wifi"]["client"]["ssid"].isNull() && !config["wifi"].isNull() && !config["wifi"]["client"]["ssid"].as<String>().isEmpty() && !config["wifi"]["client"][
        "password"].isNull());
}
