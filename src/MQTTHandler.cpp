//
// Created by JanHe on 10.01.2026.
//

#include <PubSubClient.h>
#include "MQTTHandler.h"

#include "FileHandler.h"
#include "InternalConfig.h"


PubSubClient client;

bool isEnabled = false;

// Stores last Publish Timestamp.
unsigned long previousMillis = 0;

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
 * @brief Publishes a message to a specified MQTT topic.
 *
 * This method forwards the topic and payload parameters to the internal
 * MQTT client for publishing the message. It allows sending data to a
 * subscribed topic in an MQTT broker for communication between devices.
 *
 * @param topic A C-string representing the MQTT topic to publish to.
 * @param payload A C-string containing the message payload to send.
 */
void MQTTHandler::publish(const char* topic, const char* payload)
{
    client.publish(topic, payload);
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

        unsigned long currentMillis = millis();

        // Check if the interval has passed
        if (currentMillis - previousMillis >= MQTT_INTERVAL)
        {
        }
    }
}
