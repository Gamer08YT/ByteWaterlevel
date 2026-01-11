//
// Created by JanHe on 09.01.2026.
//

#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <ArduinoJson.h>


class WiFiClient;

class WiFiHandler
{
private:
    static void checkConnection();
    static void startAP(JsonDocument& config);
    static unsigned long connectionStartTime;
    static bool apStarted;
    static const unsigned long CONNECTION_TIMEOUT_MS;
    static WiFiClient* instance;


public:
    static bool isWiFiClientUsable();
    static void setup();
    static void loop();
    static bool isConnected();
    static WiFiClient& getInstance();

};


#endif //WIFIHANDLER_H