//
// Created by JanHe on 09.01.2026.
//

#include "FileHandler.h"
#include <HardwareSerial.h>

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
        Serial.println("LittleFS failed to mount");
        return;
    }

    Serial.println("LittleFS mounted");
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
