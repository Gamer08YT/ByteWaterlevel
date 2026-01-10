//
// Created by JanHe on 09.01.2026.
//

#include <Arduino.h>
#include "WebHandler.h"
#include <InternalConfig.h>
#include <LittleFS.h>

#include "DeviceHandler.h"
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

/**
 * Sends an HTTP 400 Bad Request response to the client with a predefined error message.
 *
 * This method is called to notify the client that the request was invalid. The response is sent
 * in JSON format with an error message indicating the issue.
 *
 * @param request Pointer to the asynchronous web server request.
 */
void WebHandler::sendInvalid(AsyncWebServerRequest* request)
{
    sendResponse(request, 400, "application/json", "{type: 'error', message: 'Invalid request'}");
}

/**
 * Sends an HTTP 200 OK response to the client with a predefined success message.
 *
 * This method is used to indicate successful processing of a request. The response
 * includes a JSON-formatted message confirming the success.
 *
 * @param request Pointer to the asynchronous web server request.
 */
void WebHandler::sendOK(AsyncWebServerRequest* request)
{
    sendResponse(request, 200, "application/json", "{type: 'success', message: 'OK'}");
}

/**
 * Handles incoming API requests and processes JSON data based on the specified request type.
 *
 * This method is responsible for processing API calls and delegating the request to the appropriate
 * handler based on the "type" field in the provided JSON. It validates the request parameters and
 * sends responses based on the API logic.
 *
 * @param request Pointer to the asynchronous web server request.
 * @param json The JSON payload of the incoming API request, containing the "type" field and any associated parameters.
 */
void WebHandler::handleAPICall(AsyncWebServerRequest* request, JsonVariant json)
{
    String type = json["type"].as<String>();

    if (type == "relais")
    {
        // Check if Relais Query Contains a channel (int) and state (bool).
        if (json["channel"].is<int>() && json["state"].is<bool>())
        {
            DeviceHandler::setRelais(json["channel"].as<int>(), json["state"].as<bool>());

            sendOK(request);
        }
        else
        {
            sendInvalid(request);
        }
    }
    else
    {
        sendResponse(request, 400, "application/json", "{type: 'error', message: 'Not implemented'}");
    }
}

/**
 * Sends an HTTP response to the client with the specified status code and content.
 *
 * This method sends a response to the client using the provided status code and content type.
 * The content type is referenced by the text parameter, and the string parameter is not used
 * in the function.
 *
 * @param request Pointer to the asynchronous web server request.
 * @param i The HTTP status code to be sent in the response.
 * @param str Placeholder parameter, currently not used within the implementation.
 * @param text The MIME type of the response content.
 */
void WebHandler::sendResponse(AsyncWebServerRequest* request, int i, const char* str, const char* text)
{
    request->send(i, text);
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
        sendResponse(request, 400, "application/json",
                     "{type: 'error', message: 'No JSON payload provided.'}");

        return false;
    }
    return true;
}
