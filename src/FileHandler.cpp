//
// Created by JanHe on 09.01.2026.
//

#include "FileHandler.h"

#include <FS.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

/**
 * @brief Initializes the file system using LittleFS.
 *
 * This method attempts to mount the LittleFS file system and checks for
 * successful initialization. If the mounting fails, a failure message
 * is printed via Serial. If successful, a success message is printed.
 */
void FileHandler::begin()
{
    if (!LittleFS.begin(true))
    {
        #if DEBUG == true
                Serial.println("LittleFS failed to mount");
        #endif
        return;
    }

#if DEBUG == true
        Serial.println("LittleFS mounted");
#endif
}

/**
 * @brief Reads the content of a file from the LittleFS file system.
 *
 * This method opens the specified file in read mode, retrieves its content
 * as a string, and then closes the file. If the file cannot be opened, an error
 * message is logged via Serial, and an empty string is returned.
 *
 * @param path The file path as a C-style string to be read from the file system.
 * @return The content of the file as a String. Returns an empty string if the
 * file cannot be opened or does not exist.
 */
String FileHandler::readFile(const char* path)
{
    File file = LittleFS.open(path, "r");

    if (!file)
    {
        Serial.println("Failed to open file");
        return "";
    }

    String content = file.readString();
    file.close();
    return content;
}

/**
 * @brief Loads the configuration settings from a file.
 *
 * This method is responsible for handling the process of reading and
 * parsing the configuration file stored in the file system. It ensures
 * that the application retrieves and utilizes the necessary settings
 * for its operation based on the configuration data.
 */
bool FileHandler::loadConfig()
{
    // File file = LittleFS.open("/config.json", "r");
    // if (!file) {
    //     Serial.println("❌ Failed to open config file");
    //     return false;
    // }
    //
    // StaticJsonDocument<512> doc;
    // DeserializationError err = deserializeJson(doc, file);
    // file.close();
    //
    // if (err) {
    //     Serial.print("❌ JSON parse error: ");
    //     Serial.println(err.c_str());
    //     return false;
    // }
    //
    // // Root
    // config.name = doc["name"].as<String>();
    //
    // // WiFi client
    // config.wifi.client.ssid =
    //     doc["wifi"]["client"]["ssid"].as<String>();
    // config.wifi.client.password =
    //     doc["wifi"]["client"]["password"].as<String>();
    //
    // // WiFi AP
    // config.wifi.ap.password =
    //     doc["wifi"]["ap"]["password"].as<String>();
    //
    // // Hardware
    // config.hardware.led =
    //     doc["hardware"]["led"] | false; // default false

    return true;
}
