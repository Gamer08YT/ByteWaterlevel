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


/**
 * @brief Copies the contents of one file to another.
 *
 * This method reads the contents of the source file and writes them
 * to the destination file. If the debug mode is enabled, a success
 * message is printed to Serial.
 *
 * @param source The path to the source file to be copied.
 * @param destination The path to the destination file where the contents
 * will be written.
 * @return true if the file has been successfully copied.
 */
bool FileHandler::copyFile(const char* source, const char* destination)
{
    saveFile(destination, readFile(source));

#if DEBUG == true
    Serial.println("File successfully created.");
#endif

    return true;
}

/**
 * @brief Resets the configuration file to its backup state.
 *
 * This method replaces the current configuration file with its backup version
 * by copying "/config.json.bak" to "/config.json", effectively restoring
 * the settings to a known backup state.
 */
void FileHandler::reset()
{
    copyFile("/config.json.bak", "/config.json");
}

/**
 * @brief Merges configuration data from a backup file into the main configuration.
 *
 * Reads a backup JSON file and the main configuration JSON file, then merges
 * missing keys or values from the backup into the main configuration. Finally,
 * saves the updated configuration back to the file system.
 *
 * @param configPath The file path to the main configuration file.
 * @param backupPath The file path to the backup configuration file.
 * @return True if the merge and update process completes successfully, false otherwise.
 */
bool FileHandler::mergeConfigFromBackup(const char* configPath, const char* backupPath)
{
    // Load Backup File.
    File backupFile = LittleFS.open(backupPath, "r");
    if (!backupFile)
    {
        Serial.println("F404");
        return false;
    }

    // Parse JSON from Backup.
    JsonDocument backupDoc;
    DeserializationError backupError = deserializeJson(backupDoc, backupFile);
    backupFile.close();

    if (backupError)
    {
        Serial.print("B500");
        Serial.println(backupError.c_str());
        return false;
    }

    // Load Primary File.
    File configFile = LittleFS.open(configPath, "r");
    JsonDocument configDoc;

    if (configFile)
    {
        DeserializationError configError = deserializeJson(configDoc, configFile);
        configFile.close();

        if (configError)
        {
            Serial.print("P500 ");
            Serial.println(configError.c_str());
            configDoc.clear();
        }
    }

    // Add missing Keys.
    mergeJsonObjects(configDoc, backupDoc);

    // Save missing Keys.
    File outputFile = LittleFS.open(configPath, "w");
    if (!outputFile)
    {
        Serial.println("W500");
        return false;
    }

    serializeJsonPretty(configDoc, outputFile);
    outputFile.close();

    Serial.println("WOK");
    return true;
}


/**
 * @brief Merges two JSON objects recursively.
 *
 * This function merges key-value pairs from a source JSON object into a target JSON object.
 * If the same key exists in both objects and its value is a nested JSON object, the function
 * performs a recursive merge. For keys present in the source but absent in the target, it
 * directly adds those keys into the target object.
 *
 * @param target The target JSON object where source key-value pairs are merged into.
 * @param source The source JSON object containing key-value pairs to be merged.
 *
 * @details
 * - If the source is not a JSON object, the function exits without making any changes.
 * - For keys that point to nested JSON objects in the source, the function creates a
 *   nested object in the target (if not present) and merges recursively.
 * - Non-overlapping keys in the source are directly added to the target.
 */
void FileHandler::mergeJsonObjects(JsonVariant target, JsonVariant source)
{
    if (!source.is<JsonObject>())
    {
        return;
    }

    for (JsonPair p : source.as<JsonObject>())
    {
        if (source[p.key()].is<JsonObject>())
        {
            // If Primary Config is missing Object, add new one.
            if (!target[p.key()].is<JsonObject>())
            {
                target[p.key()] = target.to<JsonObject>().createNestedObject(p.key());
            }

            // Rekursiv merge
            mergeJsonObjects(target[p.key()], source[p.key()]);
        }
        else
        {
            // If target is missing Key, add key.
            if (!target.containsKey(p.key()))
            {
                target[p.key()] = source[p.key()];
            }
        }
    }
}
