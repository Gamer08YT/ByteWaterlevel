//
// Created by JanHe on 25.01.2026.
//

#include "OTAHandler.h"
#include <ArduinoOTA.h>

#include "esp32FOTA.hpp"
#include "FileHandler.h"
#include "InternalConfig.h"

// Store OTA Server State.
bool otaEnabled = false;

// Store Pull Instance.
esp32FOTA pull("stable", VERSION, false, true);

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
    // Local OTA Server.
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

    // Remote OTA Server.
    // Set Manifest URL.
    pull.setManifestURL(UPDATE_SERVER);
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


    if (checkWAN())
    {
        // Handle Pull.
        pull.handle();
    }
}

/**
 * @brief Checks the current Wi-Fi connection status.
 *
 * This function verifies if the device is connected to a Wi-Fi network
 * by checking the status of the Wi-Fi connection. It returns true
 * if the Wi-Fi connection is established, otherwise false.
 *
 * @return `true` if Wi-Fi is connected (`WL_CONNECTED`), otherwise `false`.
 *
 * @note This function primarily relies on the `WiFi.status()` method
 *       to determine the connectivity state.
 */
bool OTAHandler::checkWAN()
{
    return WiFi.status() == WL_CONNECTED;
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
    if (checkWAN())
    {
        return pull.execHTTPcheck();
    }

    return false;
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
 * @note Ensure a stable network connection is available for contacting the
 *       update server and downloading the firmware.
 *
 * @see ESP32OTAPull::CheckForOTAUpdate
     */
void OTAHandler::update()
{
    pull.execOTA();
}
