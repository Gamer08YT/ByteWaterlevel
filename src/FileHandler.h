//
// Created by JanHe on 09.01.2026.
//

#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <WString.h>

#include "ArduinoJson/Document/JsonDocument.hpp"


class FileHandler
{
public:
    static void begin();
    static String readFile(const char* path);
    static ArduinoJson::JsonDocument loadConfig();
    static void saveFile(const char* str, const String& string);
    static ArduinoJson::JsonDocument getConfig();
    static void saveConfig(ArduinoJson::JsonObject object);
    static ArduinoJson::JsonDocument config;
};


#endif //FILEHANDLER_H
