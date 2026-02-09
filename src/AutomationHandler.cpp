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
float fillL;

bool fillM = false;
bool pumpM = false;

// Store last Auto Timestamp.
unsigned long autoMillis = 0;

/**
 * Controls the state of the pump by interacting with the relay and updates
 * the internal state flag.
 *
 * This method uses the `DeviceHandler::setRelais` function to change the state of
 * the pump relay and modifies the internal `pumpM` flag to reflect the pump's current
 * operational status.
 *
 * @param cond Specifies the desired state of the pump.
 *             Set to `true` to activate the pump, or `false` to deactivate it.
 */
void AutomationHandler::setPump(bool cond)
{
    DeviceHandler::setRelais(PUMP_EMPTY, cond);

    pumpM = cond;
}

/**
 * Manages the operation of the pump based on the current level compared to the minimum level.
 *
 * This method evaluates the cached level reading using `DeviceHandler::getLevelCached()`
 * and determines whether the pump should be activated or deactivated. If the current
 * level is greater than the predefined minimum level, the pump is turned on by calling
 * `setPump(true)`. Otherwise, the pump is turned off by calling `setPump(false)`.
 */
void AutomationHandler::handlePump()
{
    // If the current Level is smaller than max Level.
    if (DeviceHandler::getLevelCached() > minL)
    {
        // Enable Pump Mode.
        setPump(true);
    }
    else
    {
        // Disable Pump Mode.
        setPump(false);
    }
}


/**
 * Activates or deactivates the filling system by controlling the associated pump.
 *
 * This method uses the `DeviceHandler::setRelais` function to set the state of the
 * filling pump relay. Additionally, it updates an internal flag to reflect the
 * current state of the filling system.
 *
 * @param cond Specifies whether to enable or disable the filling system.
 *             Set to `true` to activate the fill pump, or `false` to deactivate it.
 */
void AutomationHandler::setFill(bool cond)
{
    DeviceHandler::setRelais(PUMP_FILL, cond);

    fillM = cond;
}

/**
 * Manages the automated filling process based on the current sensor readings
 * and predefined thresholds.
 *
 * This method evaluates the current level of a liquid or resource and decides
 * whether to activate or deactivate the fill mode. If the current level is
 * below a predefined maximum level (`maxL`) and fill mode is not already active,
 * it checks whether the level is also below the fill threshold (`fillL`).
 * If so, it enables the filling process by invoking `setFill(true)` to initiate
 * filling. Conversely, if the level exceeds the maximum threshold, it disables
 * the filling process by calling `setFill(false)`.
 *
 * This method ensures that filling is controlled only when necessary, thus
 * preventing overfilling while maintaining the resource level within the
 * specified range.
 */
void AutomationHandler::handleFill()
{
    // If the current Level is smaller than max Level.
    if (DeviceHandler::getLevelCached() < maxL)
    {
        // Start Filling, is offset has reached.
        // For example, enableFill = (currentLevel < maxLevel && currentLevel < fillLevel).
        //
        // Example fillLevel = 50 and  maxLevel = 100
        // currentLevel = 90 = false
        // currentLevel = 100 = false
        // currentLevel = 40 =
        if (fillM == false && DeviceHandler::getLevelCached() <= fillL)
        {
            // Fill up to maxLevel and then disable Fill Mode.
            setFill(true);
        }
    }
    else
    {
        // Stopp Filling, because Level has exceeded.
        setFill(false);
    }
}

/**
 * Executes the automation loop, handling periodic tasks based on the current
 * automation mode. The loop checks elapsed time against a predefined interval
 * and triggers specific actions depending on the configured mode.
 *
 * Modes:
 * - 0: Off (No actions taken).
 * - 1: Fill & Pump (Both filling and pumping actions are handled in sequence).
 * - 2: Fill (Only the filling action is handled).
 *
 * This method ensures that fill and pump actions are executed only if the
 * elapsed time since the last execution exceeds the defined `AUTO_INTERVAL`.
 * The `autoMillis` timestamp is updated after each execution cycle to track
 * the timing.
 *
 * Internally, the following actions are performed based on the mode:
 * - Mode 1: Calls `handleFill()` followed by `handlePump()` to manage tank
 *   levels by first filling and then pumping.
 * - Mode 2: Calls `handleFill()` only to manage filling of the tank.
 * - Mode 0 or any other value: No action is taken.
 *
 * This method is designed to operate continuously in the program's main loop
 * and is not meant to be called explicitly in user code.
 */
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
 * Initializes the automation settings by loading configuration values
 * from a configuration file and assigning them to internal variables.
 *
 * This method retrieves the automation parameters `mode`, `maxL`,
 * `minL`, and `fillL` from a JSON configuration file using the
 * `FileHandler::getConfig` function. These parameters are critical in
 * determining the operational behavior of the automation system.
 */
void AutomationHandler::setup()
{
    mode = FileHandler::getConfig()["auto"]["mode"].as<int>();
    maxL = FileHandler::getConfig()["auto"]["max"].as<float>();
    minL = FileHandler::getConfig()["auto"]["min"].as<float>();
    fillL = FileHandler::getConfig()["auto"]["fill"].as<float>();
}
