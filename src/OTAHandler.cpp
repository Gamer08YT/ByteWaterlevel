//
// Created by JanHe on 25.01.2026.
//

#include <ArduinoOTA.h>
#include "OTAHandler.h"
#include <WiFi.h>
#include <ESP32OTAPull.h>
#include "FileHandler.h"
#include "InternalConfig.h"

// Store OTA Server State.
bool otaEnabled = false;

// Store Pull Instance.
ESP32OTAPull pull;

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
        otaEnabled = true;

        // Set OTA Hostname.
        ArduinoOTA.setHostname(FileHandler::getConfig()["wifi"]["ap"]["ssid"].as<String>().c_str());

        // Enable MDNS.
        ArduinoOTA.setMdnsEnabled(true);

        // Reboot on Success.
        ArduinoOTA.setRebootOnSuccess(true);

        JsonDocument doc = FileHandler::getConfig();

        if (doc["admin"]["state"].as<bool>())
        {
            // Set the Password for OTA.
            ArduinoOTA.setPassword(doc["admin"]["password"].as<String>().c_str());
        }

#if DEBUG == true
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
        {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onStart([]() { Serial.println("\nStart"); });
        ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
#endif

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
    if (otaEnabled)
    {
        // Handle incoming OTA.
        ArduinoOTA.handle();
    }
}

/**
 * @brief Checks if a new firmware update is available on the update server.
 *
 * This function communicates with the update server to determine if a newer
 * version of the firmware is available. It compares the current firmware
 * version with the available version on the server without initiating
 * the update process.
 *
 * The function uses the `ESP32OTAPull` class to check for updates by querying
 * the server defined by `UPDATE_SERVER` and comparing it to the software
 * version defined by `VERSION`. The check only determines the availability
 * of an update without performing the update itself.
 *
 * @return True if an update is available; otherwise, false.
 */
bool OTAHandler::hasUpdate()
{
    return pull.CheckForOTAUpdate(UPDATE_SERVER, VERSION, ESP32OTAPull::DONT_DO_UPDATE) ==
        ESP32OTAPull::UPDATE_AVAILABLE;
}

/**
 * @brief Initiates the OTA update process by checking for available updates.
 *
 * This function uses the `ESP32OTAPull` library to check for firmware updates
 * from a specified update server. It compares the current firmware version
 * against the versions available on the server and decides whether to update
 * and boot the device into the new firmware.
 *
 * The update server and current version are defined in the configuration
 * headers as `UPDATE_SERVER` and `VERSION`, respectively. The update action
 * performed is specified as `ESP32OTAPull::UPDATE_AND_BOOT`, which updates
 * the firmware and reboots the device to apply the update.
 *
 * @note Ensure a stable*/
void OTAHandler::update()
{
    pull.CheckForOTAUpdate(UPDATE_SERVER, VERSION, ESP32OTAPull::UPDATE_AND_BOOT);
}
