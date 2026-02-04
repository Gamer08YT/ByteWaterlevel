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

/**
 * @brief Sets up the MatterHandler by initializing its plugins and starting the Matter
 *        protocol.
 *
 * This method performs the following operations:
 * - Registers and initializes the Matter plugins via the beginPlugins method, which
 *   configures and links the required relay devices.
 * - Starts the Matter protocol stack, enabling Matter-based communication and functionality.
 *
 * This is a critical setup routine that ensures the MatterHandler is ready for operation
 * and capable of handling Matter communication. It orchestrates the initialization of
 * necessary components for Matter compliance.
 */
void MatterHandler::setup()
{
    // Register Matter Plugins.
    beginPlugins();

    // Begin Matter.
    Matter.begin();
}

/**
 * @brief Executes a continuous loop for processing or managing tasks.
 *
 * This method is designed to run indefinitely unless interrupted or terminated externally.
 * Within the loop, it can perform repetitive or necessary operations such as monitoring
 * conditions, handling events, or managing resources. The specific functionality inside
 * the loop depends on the implementation details of the application.
 *
 * Typical use cases include managing state, polling input or sensors, and executing
 * periodic actions. Care must be taken to include mechanisms for gracefully breaking or
 * exiting the loop if required.
 */
void MatterHandler::loop()
{
    // Check Matter Plugin Commissioning state, which may change during execution of loop()
    if (Matter.isDeviceConnected())
    {
        relay1.updateAccessory();
        relay2.updateAccessory();
    }
}

/**
 * @brief Retrieves the manual pairing code for the Matter device.
 *
 * This method fetches the manual pairing code that can be used to pair
 * the Matter device with a controller or ecosystem. The pairing code
 * is fetched directly from the Matter instance.
 *
 * @return A string representing the manual pairing code for the Matter device.
 */
String MatterHandler::getPairingCode()
{
    return Matter.getManualPairingCode();
}
