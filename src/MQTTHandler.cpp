//
// Created by JanHe on 10.01.2026.
//

#include "MQTTHandler.h"
#include "DeviceHandler.h"
#include "FileHandler.h"
#include "InternalConfig.h"
#include "WiFiHandler.h"
#include <espMqttClientAsync.h>

espMqttClientAsync client;

bool isEnabled = false;

// Stores last Publish Timestamp.
unsigned long previousMillisMQTT = 0;

// Stores last Reconnect Timestamp.
unsigned long reconnectMQTT = 0;

/**
 * @brief Sets the MQTT Last Will and Testament (LWT) message.
 *
 * This method configures the Last Will and Testament (LWT) for the MQTT client.
 * If the client disconnects unexpectedly, the LWT message will be published
 * to the specified topic. It is typically used to notify others of the client's
 * offline status.
 *
 * Specifically, this method sets the will message with the following parameters:
 * - Topic: "status"
 * - QoS: 1
 * - Retained: true
 * - Payload: "offline"
 */
void MQTTHandler::setLastWill()
{
    client.setWill("waterlevel/status", 1, true, "offline");
}

void MQTTHandler::setup()
{
    JsonDocument config = FileHandler::getConfig();

    if (config["mqtt"]["state"].as<bool>())
    {
        // Set Enabled State.
        isEnabled = true;

        reconnectMQTT = millis();

        // Store strings in local variables to prevent temporary object destruction
        String mqttHost = config["mqtt"]["host"].as<String>();
        String mqttUser = config["mqtt"]["user"].as<String>();
        String mqttPassword = config["mqtt"]["password"].as<String>();
        int mqttPort = config["mqtt"]["port"].as<int>();

        // Check if Hostname is Set.
        if (mqttHost.length() > 0 && mqttPort > 0)
        {
            // Set Client Destination.
            client.setServer(mqttHost.c_str(), mqttPort);

            // Set Keep-Alive Interval.
            client.setKeepAlive(120);

            // Set Client ID.
            client.setClientId("walterlevel");

            // Check for Username otherwise use Anonymous.
            if (mqttUser.length() > 0)
            {
                client.setCredentials(mqttUser.c_str(), mqttPassword.c_str());

#if DEBUG == true
                Serial.printf("MQTT %s PW %s\n", mqttUser.c_str(), mqttPassword.c_str());
#endif
            }

#if DEBUG == true
            Serial.printf("MQTT connecting to %s:%d\n", mqttHost.c_str(), mqttPort);
#endif


            // Set Last Will.
            setLastWill();

            // Add Connect Listener.
            client.onConnect([](bool sessionPresent)
            {
                Serial.printf("MQTT connected, sessionPresent=%s\n", (sessionPresent ? "true" : "false"));

                // Reset last Will.
                publish("waterlevel/status", "online");
            });

            // Add Disconnect Listener.
            client.onDisconnect([](espMqttClientTypes::DisconnectReason disconnect_reason)
            {
                Serial.printf("MQTT disconnected, reason=%d\n", disconnect_reason);
            });

            // Connect to Server.
            client.connect();

#if DEBUG == true
            Serial.println("MQTT started");
#endif
        }
        else
            Serial.println("MQTT config");
    }
    else
    {
        Serial.println("MQTT disabled");
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

#if DEBUG == true
    Serial.printf("Publish %s: %s\n", topic, payload);
#endif
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

        if (isConnected())
        {
            // Check if the interval has passed
            if (currentMillis - previousMillisMQTT >= MQTT_INTERVAL)
            {
                previousMillisMQTT = currentMillis;

                // Voltage (Float)
                publish("waterlevel/voltage", String(DeviceHandler::getADCValue(), 2).c_str());

                // Channel 1 (Bool)
                publish("waterlevel/channel/1/state", DeviceHandler::getState(1) ? "1" : "0");

                // Channel 1 Duration  (Int)
                publish("waterlevel/channel/1/duration", String(DeviceHandler::getDuration(1)).c_str());

                // Channel 2 (Bool)
                publish("waterlevel/channel/2/state", DeviceHandler::getState(2) ? "1" : "0");

                // Channel 2 Duration  (Int)
                publish("waterlevel/channel/2/duration", String(DeviceHandler::getDuration(2)).c_str());

                // CPU Temperature
                publish("waterlevel/cpu", String(DeviceHandler::getCPUTemperature(), 1).c_str());
            }
        }
        else
        {
            if (currentMillis - reconnectMQTT > 5000)
            {
                // Rerun Setup.
                setup();
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
