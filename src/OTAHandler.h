//
// Created by JanHe on 25.01.2026.
//

#ifndef OTAHANDLER_H
#define OTAHANDLER_H
#include "ArduinoJson/Document/JsonDocument.hpp"


class OTAHandler
{
public:
    static void setup();
    static void loop();
    static bool hasUpdate();
    static void update();
};


#endif //OTAHANDLER_H
