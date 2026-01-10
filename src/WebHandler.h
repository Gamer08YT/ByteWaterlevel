//
// Created by JanHe on 09.01.2026.
//

#ifndef WEBHANDLER_H
#define WEBHANDLER_H
#include "ESPAsyncWebServer.h"


class WebHandler
{
public:
    static void setup();
    static void loop();

private:
    static void handleAPICall(AsyncWebServerRequest* request, JsonVariant json);
    static bool checkRequest(AsyncWebServerRequest* request, JsonVariant json);
};


#endif //WEBHANDLER_H
