//
// Created by JanHe on 09.01.2026.
//

#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <Arduino.h>
#include <ArduinoJson.h>


class FileHandler
{
public:
    static void begin();
    static String readFile(const char* path);
    static JsonDocument loadConfig();
    static void saveFile(const char* str, const String& string);
    static JsonDocument getConfig();
    static void saveConfig(JsonObject object);
    static bool copyFile(const char* source, const char* destination);
    static JsonDocument config;
};


#endif //FILEHANDLER_H
