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
