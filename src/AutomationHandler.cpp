//
// Created by JanHe on 09.02.2026.
//

#include "AutomationHandler.h"

#include "DeviceHandler.h"
#include "FileHandler.h"
#include "InternalConfig.h"

int mode;
float maxL;
float minL;

// Store last Auto Timestamp.
unsigned long autoMillis = 0;

void AutomationHandler::handlePump()
{
}

void AutomationHandler::handleFill()
{
    // If the current Level is smaller than max Level.
    if (DeviceHandler::getLevelCached() < maxL)
    {
    }
}

void AutomationHandler::loop()
{
    /*
     * <option value="0">Off</option>
     * <option value="1">Fill & Pump</option>
     * <option value="2">Fill</option>
     */
    if (mode != 0)
    {
        unsigned long currentMillis = millis();

        // Check if the interval has passed
        if (currentMillis - autoMillis >= AUTO_INTERVAL)
        {
            autoMillis = currentMillis;

            switch (mode)
            {
            case 1:
                handleFill();
                handlePump();
                break;
            case 2:
                handleFill();
                break;
            default:
                // Do nothing.
                break;
            }
        }
    }
}

/**
 * Configures the automation settings by retrieving and storing values from the configuration file.
 *
 * This method reads specific configuration parameters from the JSON configuration
 * loaded via the FileHandler class. These parameters include:
 * - The mode for automation.
 * - The maximum limit for operations.
 * - The minimum limit for operations.
 *
 * It sets these parameters into internal variables for later use within the
 * automation system.
 *
 * @note This function must be called after the configuration has been successfully
 * loaded using the `FileHandler::loadConfig()` method.
 *
 * @warning Ensure the configuration file contains the expected structure and keys,
 * as any missing keys or mismatched types may result in runtime errors.
 */
void AutomationHandler::setup()
{
    mode = FileHandler::getConfig()["auto"]["mode"].as<int>();
    maxL = FileHandler::getConfig()["auto"]["max"].as<float>();
    minL = FileHandler::getConfig()["auto"]["min"].as<float>();
}
