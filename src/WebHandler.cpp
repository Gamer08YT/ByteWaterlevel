//
// Created by JanHe on 09.01.2026.
//

#include <Arduino.h>
#include "WebHandler.h"

#include <LittleFS.h>

#include "ESPAsyncWebServer.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void WebHandler::setup()
{
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request)
    {
        request->send(LittleFS, "/index.html", String(), false);
    });

    // Start Webserver.
    server.begin();

    #if DEBUG == true
        Serial.println("Webserver started");
    #endif
}

void WebHandler::loop()
{
}
