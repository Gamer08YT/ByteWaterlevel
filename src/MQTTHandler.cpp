//
// Created by JanHe on 10.01.2026.
//

#include "MQTTHandler.h"
#include "DeviceHandler.h"
#include "FileHandler.h"
#include "InternalConfig.h"
#include "WiFiHandler.h"
#include <AsyncMqttClient.h>

AsyncMqttClient client;

bool isEnabled = false;

// Stores last Publish Timestamp.
unsigned long previousMillisMQTT = 0;

void MQTTHandler::setup()
{
    JsonDocument config = FileHandler::getConfig();

    if (config["mqtt"]["state"].as<bool>())
    {
        // Set Enabled State.
        isEnabled = true;

        // Store strings in local variables to prevent temporary object destruction
        String mqttHost = config["mqtt"]["host"].as<String>();
        String mqttUser = config["mqtt"]["user"].as<String>();
        String mqttPassword = config["mqtt"]["password"].as<String>();
        int mqttPort = config["mqtt"]["port"].as<int>();

        // Check if Hostname is Set.
        if (mqttHost != nullptr && mqttPort)
        {
            // Set Client Destination.
            client.setServer(mqttHost.c_str(), mqttPort);

            // Set MQTT Credentials.
            client.setCredentials(mqttUser.c_str(), mqttPassword.c_str());

            // Connect to Server.
            client.connect();
        }

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
    client.publish(topic, 1, false, payload);
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
        unsigned long currentMillis = millis();

        // Check if the interval has passed
        if (currentMillis - previousMillisMQTT >= MQTT_INTERVAL)
        {
            if (client.connected())
            {
                // Voltage (Float)
                publish("waterlevel/voltage", String(DeviceHandler::getADCValue(), 2).c_str());

                // Channel 1 (Bool)
                publish("waterlevel/channel1", DeviceHandler::getState(1) ? "1" : "0");

                // Channel 2 (Bool)
                publish("waterlevel/channel2", DeviceHandler::getState(2) ? "1" : "0");
            }
        }
    }
}

/**
 * @brief Checks whether the MQTT client is currently connected.
 *
 * This method verifies the connection status of the internal MQTT client,
 * indicating if it is actively connected to an MQTT broker. It helps determine
 * the availability of the MQTT communication channel.
 *
 * @return A boolean value indicating the MQTT connection status.
 *         Returns true if the client is connected; otherwise, false.
 */
bool MQTTHandler::isConnected()
{
    return client.connected();
}
