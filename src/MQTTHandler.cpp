//
// Created by JanHe on 10.01.2026.
//

#include <PubSubClient.h>
#include "MQTTHandler.h"

#include "FileHandler.h"


PubSubClient client;

bool isEnabled = false;

void MQTTHandler::setup()
{
    JsonDocument config = FileHandler::getConfig();

    if (config["mqtt"]["state"].as<bool>())
    {
        // Set Enabled State.
        isEnabled = true;

        // Set Client Destination.
        client.setServer(config["mqtt"]["host"].as<String>().c_str(), config["mqtt"]["port"].as<int>());

        // Connect to Server.
        client.connect("waterlevel", config["mqtt"]["user"].as<String>().c_str(),
                       config["mqtt"]["password"].as<String>().c_str());

#if DEBUG == true
        Serial.println("MQTT started");
#endif
    }
}

/**
 * @brief Processes MQTT client tasks if the handler is enabled.
 *
 * This function invokes the loop method of an internal MQTT client
 * to handle any outstanding MQTT tasks (e.g., processing incoming
 * packets, managing keep-alive messages) only when the MQTTHandler
 * is enabled.
 *
 * @note Ensure the MQTTHandler is properly configured and enabled before
 * calling this function.
 */
void MQTTHandler::loop()
{
    if (isEnabled)
    {
        client.loop();
    }
}
