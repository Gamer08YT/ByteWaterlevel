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
            request->send(500, "text/plain", "Invalid LittleFS");
            return;
        }

        request->send(LittleFS, "/index.html", "text/html; charset=utf-8", false);
    });


    // Add 404 Handler.
    server.onNotFound([](AsyncWebServerRequest* request)
    {
        request->send(404, "text/plain", "Page not found");
    });

    // Add API Handler.
    server.addHandler(new AsyncCallbackJsonWebHandler("/api", [](AsyncWebServerRequest* request, JsonVariant json)
    {
        if (checkRequest(request, json))
        {
            // Parse API Type and Execute Listener.
            handleAPICall(request, json);
        };
    }));

    // Start Webserver.
    server.begin();

#if DEBUG == true
    Serial.println("Webserver started");
#endif
}

void WebHandler::loop()
{
}

void WebHandler::handleAPICall(AsyncWebServerRequest* request, JsonVariant json)
{
}

/**
 * Checks if the provided JSON payload in the request is valid.
 *
 * If the JSON variant is null, it sends a 400 Bad Request response with an error message.
 *
 * @param request Pointer to the asynchronous web server request.
 * @param json The JSON payload to be validated.
 * @return true if the JSON payload is valid, false otherwise.
 */
bool WebHandler::checkRequest(AsyncWebServerRequest* request, JsonVariant json)
{
    if (json.isNull())
    {
        request->send(400, "application/json",
                      "{type: 'error', message: 'No JSON payload provided.'}");
        return false;
    }
    return true;
}
