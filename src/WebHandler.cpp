//
// Created by JanHe on 09.01.2026.
//

#include <Arduino.h>
#include "WebHandler.h"
#include <InternalConfig.h>
#include <LittleFS.h>

#include "ESPAsyncWebServer.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void WebHandler::setup()
{
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request)
    {
        if (!LittleFS.exists("/index.html"))
        {
            request->send(500, "text/plain", "Invalid LittleFS.");
            return;
        }

        request->send(LittleFS, "/index.html", "text/html; charset=utf-8", false);
    });


    // Add 404 Handler.
    server.onNotFound([](AsyncWebServerRequest* request)
    {
        request->send(404, "text/plain", "Page not found");
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
