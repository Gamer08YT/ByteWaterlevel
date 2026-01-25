//
// Created by JanHe on 25.01.2026.
//

#include <ArduinoOTA.h>
#include "OTAHandler.h"
#include <WiFi.h>
#include "FileHandler.h"

/**
 * @brief Sets up the OTA (Over-The-Air) update functionality if enabled in
 *        the configuration.
 *
 * This function checks the configuration file to determine if OTA updates
 * are enabled. If enabled, it sets the hostname for the OTA server based
 * on the configured WiFi access point SSID and initializes the ArduinoOTA
 * server to listen for OTA updates.
 *
 * If debugging is enabled (DEBUG == true), a message is logged to the
 * Serial monitor indicating that the OTA service has started.
 *
 * @note This function depends on FileHandler's `getConfig()` method to
 *       retrieve the configuration settings, and uses the ArduinoOTA library
 *       for handling OTA updates.
 */
void OTAHandler::setup()
{
    if (FileHandler::getConfig()["ota"].as<bool>())
    {
        // Set OTA Hostname.
        ArduinoOTA.setHostname(FileHandler::getConfig()["wifi"]["ap"]["ssid"].as<String>().c_str());

        // Enable MDNS.
        ArduinoOTA.setMdnsEnabled(true);

        // Reboot on Success.
        ArduinoOTA.setRebootOnSuccess(true);

        // Begin OTA Server.
        ArduinoOTA.begin();

#if DEBUG == true
        Serial.println("OTA started");
#endif
    }
}

/**
 * @brief Processes OTA (Over-The-Air) update handling during the main
 *        application loop.
 *
 * This function calls the `ArduinoOTA.handle()` method to manage any incoming
 * OTA update requests. It ensures that the firmware remains ready to accept
 * and handle OTA updates when initiated.
 *
 * @note This function relies on the ArduinoOTA library and should be called
 *       regularly within the main application loop to facilitate uninterrupted
 *       OTA update functionality.
 */
void OTAHandler::loop()
{
    // Handle incoming OTA.
    ArduinoOTA.handle();
}
