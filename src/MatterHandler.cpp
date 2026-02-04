//
// Created by JanHe on 11.01.2026.
//

#include "MatterHandler.h"

#include <DeviceHandler.h>
#include <Matter.h>

// Store Matter Plugin Instances.
MatterOnOffPlugin relay1;
MatterOnOffPlugin relay2;

/**
 * @brief Initializes and sets up the plugins for relay devices used in the MatterHandler.
 *
 * This method begins the operation of two relay devices (relay1 and relay2) and assigns
 * listeners to them. Each listener is triggered whenever the state of its corresponding
 * relay changes, allowing the corresponding relay state to be updated via the
 * DeviceHandler::setRelais method.
 *
 * The listeners are configured as follows:
 * - Relay 1: Updates its state using DeviceHandler::setRelais(1, cond) when its state changes.
 * - Relay 2: Updates its state using DeviceHandler::setRelais(2, cond) when its state changes.
 *
 * This method ensures the relays are properly initialized and connected with their respective listeners.
 */
void MatterHandler::beginPlugins()
{
    // Begin Relais 1.
    relay1.begin();

    // Add Listener for Relais 1.
    relay1.onChange([](bool cond)
    {
        DeviceHandler::setRelais(1, cond);

        return cond;
    });

    // Begin Relais 2.
    relay2.begin();

    // Add Listener for Relais 2.
    relay2.onChange([](bool cond)
    {
        DeviceHandler::setRelais(2, cond);

        return cond;
    });
}

void MatterHandler::setup()
{
    // Register Matter Plugins.
    beginPlugins();

    // Begin Matter.
    Matter.begin();
}

void MatterHandler::loop()
{
    // Check Matter Plugin Commissioning state, which may change during execution of loop()
    if (Matter.isDeviceConnected())
    {
        relay1.updateAccessory();
        relay2.updateAccessory();
    }
}
