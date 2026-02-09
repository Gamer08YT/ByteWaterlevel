#include "Arduino.h"
#include "AutomationHandler.h"
#include "DeviceHandler.h"
#include "FileHandler.h"
#include "MQTTHandler.h"
#include "OTAHandler.h"
#include "WebHandler.h"
#include "WiFiHandler.h"
//#include <MatterHandler.h>

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
    Serial.setDebugOutput(true);

    // Setup File System.
    FileHandler::begin();

    // Load Config File.
    FileHandler::loadConfig();

    // Setup Device Pins.
    DeviceHandler::setup();

    // Setup Wi-Fi Connection from LittleFS.
    WiFiHandler::setup();

    // Setup Web.
    WebHandler::setup();

    // Setup OTA.
    OTAHandler::setup();

    // Setup Automation Handler.
    AutomationHandler::setup();

    // Setup Matter.
    //MatterHandler::setup();

    // Check for Connection.
    if (WiFiHandler::isConnected())
    {
        // Setup MQTT.
        MQTTHandler::setup();
    }

    Serial.println("Ready");
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

    // Loop OTA.
    OTAHandler::loop();

    // Loop Automation Handler.
    AutomationHandler::loop();

    // Loop Matter.
    //MatterHandler::loop();
}
