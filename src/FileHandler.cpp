//
// Created by JanHe on 09.01.2026.
//

#include "FileHandler.h"

#include <InternalConfig.h>
#include <FS.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

// Definition der statischen Member-Variable
JsonDocument FileHandler::config;


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
JsonDocument FileHandler::loadConfig()
{
    // Copy file if not exists.
    if (!LittleFS.exists("/config.json"))
    {
        copyFile("/config.json.bak", "/config.json");
    }

    // Deserialize Json Config.
    deserializeJson(config, readFile("/config.json"));

    return config;
}

/**
 * @brief Saves a string to a file in the LittleFS file system.
 *
 * This method opens or creates a file with the specified name and writes
 * the provided string into it. After writing, the file is closed.
 *
 * @param str The name or path of the file to which the string will be saved.
 * @param string The string content to be written into the file.
 */
void FileHandler::saveFile(const char* str, const String& string)
{
    File file = LittleFS.open(str, "w");
    file.println(string);
    file.close();
}

/**
 * @brief Retrieves the current configuration stored in the file handler.
 *
 * This method returns the configuration data maintained by the FileHandler
 * object, typically loaded or managed during runtime.
 *
 * @return The current configuration as a JsonDocument.
 */
JsonDocument FileHandler::getConfig()
{
    return config;
}

/**
 * @brief Saves the provided configuration into the internal configuration object.
 *
 * This method updates the internal configuration object with the values
 * provided in the input JsonObject.
 *
 * @param object The JsonObject containing the configuration to save.
 */
void FileHandler::saveConfig(JsonObject object)
{
    config.set(object);
}


bool FileHandler::copyFile(const char* source, const char* destination)
{
    saveFile(destination, readFile(source));
    
#if DEBUG == true
    Serial.println("File successfully created.");
#endif

    return true;
}
