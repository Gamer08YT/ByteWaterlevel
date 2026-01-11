//
// Created by JanHe on 09.01.2026.
//

#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H
#include <WiFiClient.h>

#include "ArduinoJson/Document/JsonDocument.hpp"


class WiFiHandler
{
private:
    static void checkConnection();
    static void startAP(ArduinoJson::JsonDocument& config);
    static unsigned long connectionStartTime;
    static bool apStarted;
    static const unsigned long CONNECTION_TIMEOUT_MS;

public:
    static bool isWiFiClientUsable();
    static void setup();
    static void loop();
};


#endif //WIFIHANDLER_H