#include "Arduino.h"
#include "DeviceHandler.h"
#include "FileHandler.h"
#include "MQTTHandler.h"
#include "WebHandler.h"
#include "WiFiHandler.h"

/**
 * @brief Initializes the system components necessary for operation.
 *
 * The setup function is responsible for:
 * 1. Initializing the serial communication at a baud rate of 115200.
 * 2. Preparing the file system for use by calling FileHandler::begin.
 * 3. Configuring the Wi-Fi connection by calling WiFiHandler::setup,
 *    using the stored configuration from the file system.
 * 4. Setting up the web components by invoking WebHandler::setup.
 *
 * It ensures that all the core subsystems are initialized before
 * entering the main loop.
 */
void setup()
{
    // Begin Serial.
    Serial.begin(115200);

    // Setup Device Pins.
    DeviceHandler::setup();

    // Setup File System.
    FileHandler::begin();

    // Load Config File.
    FileHandler::loadConfig();

    // Setup Wi-Fi Connection from LittleFS.
    WiFiHandler::setup();

    // Setup Web.
    WebHandler::setup();

    // Setup MQTT.
    MQTTHandler::setup();
}

void loop()
{
    // Handle Device Loop.
    DeviceHandler::loop();

    // Check for WiFi Connection.
    WiFiHandler::loop();

    // Handle Web.
    WebHandler::loop();

    // Loop Client.
    MQTTHandler::loop();
}
