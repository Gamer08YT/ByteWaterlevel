//
// Created by JanHe on 09.01.2026.
//

#include <Arduino.h>
#include <ArduinoJson.h>
#include "WebHandler.h"
#include <InternalConfig.h>
#include <LittleFS.h>
#include <lwip/sockets.h>

#include "DeviceHandler.h"
#include "ESPAsyncWebServer.h"
#include "FileHandler.h"
#include "MQTTHandler.h"
#include "OTAHandler.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void WebHandler::setup()
{
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request)
    {
        if (needAuth(request))
        {
            if (!LittleFS.exists("/index.html"))
            {
                request->send(500, "text/plain", "Invalid LittleFS");
                return;
            }

            request->send(LittleFS, "/index.html", "text/html; charset=utf-8", false);
        }
        // else
        // {
        //     request->send(401, "text/plain", "Unauthorized");
        // }
    });


    // Add 404 Handler.
    server.onNotFound([](AsyncWebServerRequest* request)
    {
        request->send(404, "text/plain", "Page not found");
    });

    // Add API Handler.
    server.addHandler(new AsyncCallbackJsonWebHandler("/api", [](AsyncWebServerRequest* request, JsonVariant json)
    {
        if (needAuth(request))
        {
            if (checkRequest(request, json))
            {
                // Parse API Type and Execute Listener.
                handleAPICall(request, json);
            }
        }
        // else
        //     request->send(401, "application/json", R"({"type":"error","message":"Unauthorized"})");
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
 * Determines whether authentication is needed for the incoming request and enforces it if required.
 *
 * This method checks the configuration to see if the admin authentication is enabled. If enabled,
 * it verifies the provided credentials against predefined "admin" values. If authentication fails,
 * a challenge is sent to the client to request valid credentials.
 *
 * @param request Pointer to the asynchronous web server request being processed.
 * @return True if the request is authenticated or authentication is not required;
 *         false if authentication is required and the provided credentials are invalid.
 */
bool WebHandler::needAuth(AsyncWebServerRequest* request)
{
    JsonDocument doc = FileHandler::getConfig();

    if (doc["admin"]["state"].as<bool>())
    {
        if (!request->authenticate(doc["admin"]["user"], doc["admin"]["password"]))
        {
            request->requestAuthentication();
            return false;
        }
    }

    return true;
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
    sendResponse(request, 400, R"({"type":"error","message":"Invalid request"})");
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
    sendResponse(request, 200, R"({"type":"success","message":"OK"})");
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
            // Set Relais State.
            DeviceHandler::setRelais(json["channel"].as<int>(), json["state"].as<bool>());

            // Auto Disable Relais if set.
            if (json["duration"].is<int>())
            {
                // Switch off Relais after given Duration.
                DeviceHandler::setRelaisDuration(json["channel"].as<int>(), json["duration"].as<int>());
            }

            // Send 200 Response.
            sendOK(request);
        }
    }
    else if (type == "status")
    {
        JsonDocument doc;

        // Set Response Type.
        doc["type"] = "success";

        // Add Channel States to Array.
        doc["channels"][0] = DeviceHandler::getState(1);
        doc["channels"][1] = DeviceHandler::getState(2);


        // Set ADC Voltage.
        doc["adc"] = DeviceHandler::getADCValue();

        // Set Current (eq. 4-20mA).
        doc["current"] = DeviceHandler::getCurrent(false);

        // Set Tank Level in %.
        doc["level"] = DeviceHandler::getLevel();

        // Set Volume in L.
        doc["volume"] = DeviceHandler::getVolume();

        // Set CPU Temperature.
        doc["cpu"] = DeviceHandler::getCPUTemperature();

        // Set CPU Frequency.
        doc["frequency"] = ESP.getCpuFreqMHz();

        // Set MQTT State.
        doc["mqtt"] = MQTTHandler::isConnected();

        // Set Runtime.
        doc["up"] = millis() / 1000;

        String response;
        serializeJson(doc, response);
        sendResponse(request, 200, response.c_str());
    }
    else if (type == "info")
    {
        JsonDocument doc;

        // Set Config.
        doc["config"] = FileHandler::getConfig();

        // Add Firmware Info
        doc["firmware"] = VERSION;

        // Add Update Info.
        doc["update"] = OTAHandler::hasUpdate();

        // Set Response Type.
        doc["type"] = "success";

        String response;
        serializeJson(doc, response);
        sendResponse(request, 200, response.c_str());
    }
    else if (type == "save")
    {
        if (json["config"].is<JsonObject>())
        {
            String response;
            serializeJson(json["config"], response);

            // Save File to Flash.
            FileHandler::saveFile("/config.json", response);

            // Save Config.
            FileHandler::saveConfig(json["config"]);

            // Send 200 as Response.
            sendOK(request);
        }
        else
            sendInvalid(request);
    }
    else if (type == "restart")
    {
        // Send 200 as Response.
        sendOK(request);

        // Wait for 500ms.
        delay(500);

        // Restart ESP.
        ESP.restart();
    }
    else if (type == "update")
    {
        if (OTAHandler::hasUpdate())
        {
            // Send 200 as Response.
            sendOK(request);

            // Update to Latest Version.
            OTAHandler::update();
        }
        else
        {
            // Send no Update Available Response.
            sendResponse(request, 400, R"({"type":"error","message":"No update available"})");
        }
    }
    else if (type == "reset")
    {
        // Send 200 as Response.
        sendOK(request);

        // Copy Backup Config to config.json.
        FileHandler::reset();

        // Wait for 1 Second.
        delay(1000);

        // Restart ESP.
        ESP.restart();
    }
    else
    {
        sendResponse(request, 400, R"({"type":"error","message":"Not implemented"})");
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
void WebHandler::sendResponse(AsyncWebServerRequest* request, int i, const char* text)
{
    request->send(i, "application/json", text);
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
    if (!json["type"].is<String>())
    {
        sendResponse(request, 400, R"({"type":"error","message":"No JSON payload provided"})");

        return false;
    }

    return true;
}
