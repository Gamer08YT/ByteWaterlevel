//
// Created by JanHe on 09.01.2026.
//

#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <ArduinoJson.h>


class WiFiHandler
{
private:
    static void checkConnection();
    static void startAP(JsonDocument& config);
    static unsigned long connectionStartTime;
    static bool apStarted;
    static const unsigned long CONNECTION_TIMEOUT_MS;

public:
    static bool isWiFiClientUsable();
    static void setup();
    static void loop();
};


#endif //WIFIHANDLER_H