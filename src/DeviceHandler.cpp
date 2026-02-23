//
// Created by JanHe on 10.01.2026.
//

#include "DeviceHandler.h"
#include <Arduino.h>
#include <FS.h>

#include "FileHandler.h"
#include "InternalConfig.h"

// Store last Blink Timestamp.
unsigned long previousMillis = 0;

// Store last Scan Timestamp.
unsigned long scanMillis = 0;

// Store last Relais Duration Timestamp.
unsigned long ch1 = -1;
unsigned long ch2 = -1;

// Store Relais States.
bool relay1 = LOW;
bool relay2 = LOW;


// Current LED state
bool ledState = LOW;

// Store latest Voltage.
float latestVoltage = 0.00;

// Store State of System LED.
bool systemLed = true;

// Store Calibration Values.
float maxV = 2.4;
float minV = 0.0;

// Store Tank Volume.
float tankVolume = 1000.0;

// Store scanned Values (Caching to save CPU)
float scanCurrent = 0.00;
float scanTemperature = 0.00;
float scanWaterLevel = 0.00;
float scanWaterVolume = 0.00;


/**
 * Monitors and manages the state of relays based on their configured timers.
 *
 * This method checks the elapsed time for two relay channels (`ch1` and `ch2`)
 * and switches the corresponding relay off if the configured timer (in milliseconds)
 * for that relay has expired. The relays are controlled through the `setRelais()`
 * method, which alters their state.
 *
 * Preconditions:
 * - `ch1` and `ch2` must be assigned appropriate millisecond timer values. A value of `-1`
 *   indicates that the respective channel is inactive and will not be processed.
 * - The relays must be properly initialized and capable of being toggled through the `setRelais()` method.
 *
 * Postconditions:
 * - If the current time exceeds the timer value for a given channel, the corresponding
 *   relay is switched off (set to `false`).
 *
 * Behavior:
 * - If `ch1` is active and its timer has expired, relay channel 1 is switched off.
 * - If `ch2` is active and its timer has expired, relay channel 2 is switched off.
 * - Relays with inactive or unconfigured timers (`-1`) remain unaffected.
 */
void DeviceHandler::handleRelais()
{
    unsigned long currentMillis = millis();

    // Check if the interval has passed
    if (ch1 != -1 && (currentMillis >= ch1))
    {
        setRelais(1, false);
    }

    if (ch2 != -1 && (currentMillis >= ch2))
    {
        setRelais(2, false);
    }
}

/**
 * Toggles the state of an LED at a fixed interval defined by BLINK_INTERVAL.
 *
 * This method monitors the elapsed time using the `millis()` function and
 * toggles the state of an LED connected to the pin defined by LED_PIN whenever
 * the elapsed time since the last toggle exceeds BLINK_INTERVAL. It also updates
 * the last toggle time to the current time. This ensures that the LED blinks
 * consistently at the set interval.
 *
 * Preconditions:
 * - The LED_PIN must be configured as an output pin before invoking this method.
 *
 * Postconditions:
 * - The LED state alternates between HIGH and LOW at the specified interval.
 *
 * Behavior:
 * - If the time since the last toggle is less than BLINK_INTERVAL, no action is taken.
 * - Once the interval has elapsed, the LED state is toggled, and the last toggle time is updated.
 */
void DeviceHandler::handleBlink()
{
    unsigned long currentMillis = millis();

    // Check if the interval has passed
    if (currentMillis - previousMillis >= BLINK_INTERVAL)
    {
        previousMillis = currentMillis; // Save the last toggle time

        // Toggle LED state
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
    }
}


/**
 * Executes an ADC (Analog-to-Digital Converter) reading task and terminates itself.
 *
 * This task function retrieves the latest ADC value by invoking the `DeviceHandler::getADCValue` method.
 * The resulting value is stored in the `latestVoltage` variable, serving as a reference for subsequent operations
 * or calculations. Following the read operation, the task automatically deletes itself to free up system resources.
 *
 * Preconditions:
 * - The ADC and its associated configurations (e.g., `SENSE` pin defined in `InternalConfig.h`) must be properly initialized.
 * - The `DeviceHandler` class must be accessible and configured for ADC operations.
 *
 * Postconditions:
 * - The `latestVoltage` variable contains the most recent ADC-converted voltage value.
 * - The task is deleted and will no longer consume system resources.
 *
 * Behavior:
 * - Reads an analog voltage value through the `DeviceHandler::getADCValue` method.
 * - Transfers control to the FreeRTOS scheduler for task deletion via `vTaskDelete`.
 *
 * Note:
 * - This function runs once and is self-terminating upon completion.
 */
void adcTaskFunction(void* parameter)
{
    // Read ADC First = latestVoltage as ref.
    DeviceHandler::getADCValue();

    // Task beendet sich automatisch
    vTaskDelete(NULL);
}


/**
 * Conducts a sensor scan by initiating ADC conversion and retrieving sensor values.
 *
 * This method performs multiple actions:
 * - Starts a task to initiate ADC voltage reading, updating the `latestVoltage` reference.
 * - Reads and updates various sensor-related parameters, including current, CPU temperature,
 *   water level percentage, and water volume.
 *
 * Behavior:
 * - The ADC task (`adcTaskFunction`) is created to asynchronously read the voltage via ADC,
 *   updating the `latestVoltage` value. The task is automatically destroyed after execution.
 * - The `getCurrent()` method fetches the current sensor value based on the ADC reading.
 * - The `getCPUTemperature()` method retrieves the CPU's current temperature.
 * - The `getLevel()` method calculates the water level percentage using the latest ADC voltage.
 * - The `getVolume()` method computes the water tank's volume based on the water level percentage.
 *
 * Preconditions:
 * - The ADC hardware and associated pins must be properly configured and operational.
 * - The system should support task creation using `xTaskCreate`.
 * - The `minV`, `maxV`, and `tankVolume` must be configured appropriately to ensure accurate calculations.
 *
 * Postconditions:
 * - Updates the global variables `scanCurrent`, `scanTemperature`, `scanWaterLevel`, and `scanWaterVolume`
 *   with the most recent sensor readings.
 *
 * Threading Model:
 * - Utilizes FreeRTOS task functionality (`xTaskCreate`) to perform ADC reading asynchronously.
 */
void DeviceHandler::scanSensors()
{
    // Read ADC First = latestVoltage as ref.
    xTaskCreate(
        adcTaskFunction,
        "ADC Task",
        2048,
        NULL,
        1,
        NULL
    );


    // Read Sensor Values.
    scanCurrent = roundToTwoDecimals(getCurrent(false));
    scanTemperature = roundToTwoDecimals(getCPUTemperature());
    scanWaterLevel = roundToTwoDecimals(getLevel());
    scanWaterVolume = roundToTwoDecimals(getVolume());
}

/**
 * Manages periodic sensor scanning based on a predefined time interval.
 *
 * This method verifies if the specified time interval (`SCAN_INTERVAL`) has elapsed.
 * If the interval has passed, it updates the `scanMillis` timestamp to the current time
 * and invokes the `scanSensors()` method to read sensor data.
 *
 * Preconditions:
 * - The `SCAN_INTERVAL` constant must be defined to specify the time interval between scans.
 * - Sensor-related functionalities, handled in the `scanSensors()` method, must be implemented and operational.
 *
 * Postconditions:
 * - If the interval has elapsed, sensor data is refreshed via `scanSensors()`.
 * - The `scanMillis` variable is updated to the current time to track the last scan moment.
 *
 * Behavior:
 * - Performs no operation if the elapsed time since the last scan is less than the defined interval.
 * - Triggers the sensor scanning process as soon as the interval has been satisfied.
 */
void DeviceHandler::handleScan()
{
    unsigned long currentMillis = millis();

    // Check if the interval has passed
    if (currentMillis - scanMillis >= SCAN_INTERVAL)
    {
        scanMillis = currentMillis;

        scanSensors();
    }
}

/**
 * Executes periodic tasks required for the proper functioning of the device.
 *
 * This method acts as the main loop for the `DeviceHandler` class, invoking
 * specific subroutines in a predefined sequence. Each subroutine manages
 * a distinct functionality of the device, such as blinking an LED, handling
 * relay states, and performing scanning operations.
 *
 * Behavior:
 * - Calls `handleBlink()` to manage the LED blinking operation based on the specified
 *   blink interval.
 * - Calls `handleRelais()` to monitor and control the state of relays based on their
 *   configured timers.
 * - Calls `handleScan()` to handle scanning-related tasks.
 *
 * Preconditions:
 * - The device and its components (e.g., relays, LED) must be properly initialized and
 *   functional.
 * - Any required configurations (e.g., timers, intervals) must be defined and valid.
 *
 * Postconditions:
 * - Executes the subroutines sequentially, ensuring periodic tasks are handled as needed.
 */
void DeviceHandler::loop()
{
    handleBlink();
    handleRelais();
    handleScan();
}

/**
 * Sets the state of the specified relay channel.
 *
 * This method controls the state of a relay by writing a HIGH or LOW signal
 * to its associated pin. It determines the appropriate pin based on the relay
 * channel number provided as input. Optionally, debug output is printed to the
 * serial monitor if DEBUG is enabled.
 *
 * Preconditions:
 * - RELAIS_CH1 and RELAIS_CH2 must be correctly defined as the pin numbers for relay channels 1 and 2, respectively.
 * - The associated pins for the relay channels must be configured as output using `pinMode()` before invoking this method.
 *
 * Postconditions:
 * - The specified relay's state will be set to either HIGH or LOW based on the input state.
 *
 * Behavior:
 * - If an invalid relay channel is provided, the function writes to an undefined pin.
 *
 * @param relais Specifies the relay channel to be controlled (1 or 2).
 * @param state The desired state for the relay: true for HIGH, false for LOW.
 */
void DeviceHandler::setRelais(int8_t relais, bool state)
{
    unsigned int pin;

    if (relais == 1)
    {
        pin = RELAIS_CH1;
        ch1 = -1;
        relay1 = state;
    }
    else if (relais == 2)
    {
        pin = RELAIS_CH2;
        ch2 = -1;
        relay2 = state;
    }

    // Set Relais State.
    digitalWrite(pin, state);

#if DEBUG == true
    Serial.print("Relais ");
    Serial.print(relais);
    Serial.print(" set to ");
    Serial.println(state);
#endif
}

/**
 * Configures the pin modes for the device's LED and relay channels.
 *
 * This method sets up the microcontroller's pins defined in InternalConfig.h
 * for the device's LED and relays. Each pin is configured as an output to
 * ensure proper control and operation during the program execution.
 *
 * Preconditions:
 * - The pin numbers for LED_PIN, RELAIS_CH1, and RELAIS_CH2 must be correctly
 *   defined in the configuration file (e.g., InternalConfig.h).
 *
 * Postconditions:
 * - LED_PIN is set to OUTPUT mode, enabling control of an external LED.
 * - RELAIS_CH1 and RELAIS_CH2 are set to OUTPUT mode, enabling control of
 *   external relays.
 *
 * Behavior:
 * - Each pin is configured using the `pinMode` function with the OUTPUT mode.
 * - No other initialization or state setup is performed in this method.
 */
void DeviceHandler::setup()
{
    // Setup ADC Resolution.
    analogReadResolution(12);

    // Set ADC Attenuation.
    analogSetPinAttenuation(SENSE, ADC_11db);

    // Input Pins.
    pinMode(SENSE, INPUT);

    // Output Pins.
    pinMode(LED_PIN, OUTPUT);
    pinMode(RELAIS_CH1, OUTPUT);
    pinMode(RELAIS_CH2, OUTPUT);
    pinMode(RESET, OUTPUT);

    // If Pin 9 is HIGH, reset Configuration.
    if (digitalRead(RESET))
    {
        // Print Debug Message.
        Serial.println("Reset");

        // Reset Configuration.
        FileHandler::reset();
    }

    // Set System LED State.
    systemLed = FileHandler::getConfig()["hardware"]["led"].as<bool>();
    maxV = FileHandler::getConfig()["calibration"]["max"].as<float>();
    minV = FileHandler::getConfig()["calibration"]["min"].as<float>();
    tankVolume = FileHandler::getConfig()["calibration"]["volume"].as<float>();
}

/**
 * Sets the duration for which a specific relay channel should remain active.
 *
 * This method schedules the deactivation of the specified relay channel by
 * setting an expiration time relative to the current time. The relay channel
 * will be turned off once the specified duration has elapsed.
 *
 * Preconditions:
 * - The `channel` parameter must be either 1 or 2 to correspond to valid relay channels.
 * - The `duration` parameter must specify the desired duration in milliseconds.
 *
 * Behavior:
 * - If the `channel` is 1, the expiration time for channel 1 is updated.
 * - If the `channel` is 2, the expiration time for channel 2 is updated.
 * - No action is taken if the `channel` value is invalid.
 *
 * @param channel The relay channel to set the duration for. Valid values are 1 or 2.
 * @param duration The duration in milliseconds for which the relay should remain active.
 */
void DeviceHandler::setRelaisDuration(int channel, int duration)
{
    if (channel == 1)
    {
        ch1 = millis() + (duration * 1000);
    }
    else if (channel == 2)
    {
        ch2 = millis() + (duration * 1000);
    }
}

/**
 * Retrieves the state of a specified relay.
 *
 * This method returns the current state of the relay identified by the given index.
 * The state indicates whether the relay is active (`true`) or inactive (`false`).
 *
 * Preconditions:
 * - The input index should correspond to a valid relay channel.
 * - Relay `1` corresponds to `relay1`.
 * - Relay `2` corresponds to `relay2`.
 *
 * Behavior:
 * - If `i` is `1`, the method returns the state of `relay1`.
 * - If `i` is `2`, the method returns the state of `relay2`.
 * - Undefined behavior occurs if the input index does not match a valid relay channel.
 *
 * @param i The index of the relay to retrieve the state for. Accepts `1` for `relay1`
 *          and `2` for `relay2`.
 * @return The state of the specified relay:
 *         - `true` if the relay is active.
 *         - `false` if the relay is inactive.
 */
bool DeviceHandler::getState(int i)
{
    if (i == 1)
        return relay1;
    else if (i == 2)
        return relay2;
    return false;
}

/**
 * Reads the analog value from the defined sensor pin.
 *
 * This method fetches the current analog-to-digital converted value
 * from the microcontroller's `SENSE` pin, which is mapped to a specific
 * hardware input channel. The result is a 10-bit value (0 to 1023) by default,
 * depending on the resolution and configuration of the analog-to-digital converter (ADC).
 *
 * Preconditions:
 * - The `SENSE` pin must be properly configured and connected to an analog input source.
 * - The ADC feature must be enabled and configured in the microcontroller.
 *
 * Postconditions:
 * - Returns a 16-bit unsigned value representing the current analog reading
 *   from the configured pin.
 *
 * @return The analog-to-digital converted value from the `SENSE` pin.
 */
float DeviceHandler::getADCValue()
{
    return readVoltage(SENSE, 64);
}

/**
 * Calculates and returns the current in amperes based on the ADC (Analog-to-Digital Converter) value.
 *
 * This method retrieves the ADC value using the `getADCValue()` method and converts it into a current
 * measurement by dividing the value by a constant factor (120.0). The ADC value is assumed to represent
 * a voltage, which is transformed according to the device's characteristics to estimate the current.
 *
 * Preconditions:
 * - The device must be configured to measure voltage correctly via its ADC subsystem.
 * - The `getADCValue()` method must return a valid voltage reading.
 *
 * Postconditions:
 * - Returns the calculated current as a floating-point value in amperes.
 *
 * Behavior:
 * - The method performs a voltage-to-current transformation using a constant scaling factor.
 * - No additional error handling for invalid ADC readings or division by zero is implemented.
 */
float DeviceHandler::getCurrent(bool newReading = false)
{
    return ((newReading ? getADCValue() : latestVoltage) / 120) * 1000.0;
}

/**
 * Reads and calculates the voltage at a specified analog pin by averaging multiple samples.
 *
 * This method performs multiple analog readings from the specified pin, averages these readings, and
 * calculates the corresponding voltage based on the reference voltage and ADC resolution.
 *
 * Preconditions:
 * - The specified analog pin must be properly configured and capable of returning ADC values.
 * - The number of samples must be positive. The default value is 10 samples.
 * - The ADC resolution is assumed to be 12-bit with a full-scale value of 4095, and the reference voltage is 3.3V.
 *
 * Behavior:
 * - The method performs a loop to read the ADC value `samples` times.
 * - A short delay of 10 milliseconds is applied between successive readings to reduce fluctuations.
 * - The average ADC value is computed and converted to a voltage based on the ADC characteristics.
 *
 * @param pin The analog pin to read from.
 * @param samples The number of readings to average. Default value is 10.
 * @return The calculated voltage based on the averaged ADC readings.
 */
float DeviceHandler::readVoltage(int pin, int samples = 10)
{
    int sum = 0;
    for (int i = 0; i < samples; i++)
    {
        sum += analogRead(pin);
        delay(10); // Kleine Verzögerung zwischen Messungen
    }
    int average = sum / samples;
    float voltage = average * (3.3 / 4095.0);

    latestVoltage = voltage;

    return voltage;
}


/**
 * Retrieves the current CPU temperature.
 *
 * This method fetches the current temperature of the CPU from the hardware
 * and returns it as an unsigned 16-bit integer. The value is obtained using
 * the `temperatureRead()` function, which interacts directly with the hardware
 * to measure the CPU temperature.
 *
 * Preconditions:
 * - The underlying hardware must be capable of providing temperature readings.
 * - The `temperatureRead()` function must be implemented and operational.
 *
 * Postconditions:
 * - The returned value represents the CPU temperature in the unit as defined
 *   by the `temperatureRead()` implementation.
 *
 * @return The current CPU temperature as an unsigned 16-bit integer.
 */
float DeviceHandler::getCPUTemperature()
{
    return temperatureRead();
}

/**
 * Retrieves the remaining duration (in milliseconds) for a specified relay channel.
 *
 * This method calculates the time remaining for the specified relay channel (`ch1` for `i=1`
 * or `ch2` for `i=2`) before it is expected to timeout. The duration is determined based on
 * the current time and the channel's configured timer value.
 *
 * Arguments:
 * - `i`: The index of the relay channel to query (1 for `ch1` or 2 for `ch2`).
 *
 * Behavior:
 * - If the channel's timer value is `-1`, indicating it is inactive, the returned duration
 *   will be `0`.
 * - If the channel's timer value is configured and has not expired, the method calculates
 *   the remaining duration in milliseconds by subtracting the current time (`millis()`)
 *   from the timer value.
 * - If the timer value has already expired or is less than the current time, the returned
 *   duration will be `0`.
 *
 * Returns:
 * - The time remaining in milliseconds for the specified relay channel, or `0` if the
 *   channel is inactive or its timer has already expired.
 *
 * Preconditions:
 * - `i` must be either `1` for channel `ch1` or `2` for channel `ch2`. Other values may lead
 *   to undefined results.
 * - The `millis()` function must provide a valid, non-negative time value.
 *
 * Postconditions:
 * - The method will ensure a non-negative remaining duration is returned, even in cases
 *   where the timer has expired.
 */
int DeviceHandler::getDuration(int i)
{
    int duration = 0;

    if (i == 1)
    {
        duration = (ch1 <= 0 ? 0 : ch1);
    }

    if (i == 2)
    {
        duration = (ch2 <= 0 ? 0 : ch2);
    }

    // Timestamp to Seconds.
    if (duration > 0 && (duration - millis()) > 0)
    {
        return (duration - millis());
    }

    return 0;
}

/**
 * Calculates the current level in percentage based on the latest voltage reading.
 *
 * This method computes the relative percentage of the latest voltage reading
 * (`latestVoltage`) within the specified range (`minV` to `maxV`).
 * The value is constrained to lie within the range of 0% to 100%.
 *
 * Behavior:
 * - If the latest voltage is below `minV`, the level is set to 0%.
 * - If the latest voltage is above `maxV`, the level is set to 100%.
 * - For all other values, the percentage is calculated as:
 *   `(latestVoltage - minV) / (maxV - minV) * 100.0f`.
 *
 * Preconditions:
 * - `minV` and `maxV` define the valid range for voltage values, where `minV < maxV`.
 *
 * Postconditions:
 * - The returned value is a floating-point percentage value within the range of 0 to 100.
 *
 * @return The calculated level as a percentage within the range of 0% to 100%.
 */
float DeviceHandler::getLevel()
{
    float percent = (latestVoltage - minV) / (maxV - minV) * 100.0f;

    // Limit to 0–100 %
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 100.0f) percent = 100.0f;

    return percent;
}

/**
 * Calculates the current volume of liquid in the tank.
 *
 * This method determines the volume of liquid in the tank by using the
 * predefined total tank capacity (`tankVolume`) and the current liquid
 * level percentage (`getLevel()`), returning the resulting volume in
 * liters. The level percentage is expected to be a value between 0 and 100.
 *
 * @return The calculated volume of liquid in the tank, based on the current level and tank capacity.
 */
float DeviceHandler::getVolume()
{
    return tankVolume * (getLevel() / 100.0f);
}

/**
 * Retrieves the cached value of the water volume measurement.
 *
 * This method returns the most recently scanned water volume, stored in the `scanWaterVolume`
 * variable. The value represents the last known measurement without triggering a new
 * volume scanning operation. It is useful for applications where frequent or real-time
 * volume updates are not required, thus saving computational resources.
 *
 * @return The cached water volume as a floating-point value. The value is the last recorded
 *         measurement of the water volume or the default initialized value if no update
 *         has occurred.
 */
float DeviceHandler::getVolumeCached()
{
    return scanWaterVolume;
}

/**
 * Retrieves the cached value of the CPU temperature.
 *
 * This method provides the most recently scanned value of the CPU temperature,
 * stored in the `scanTemperature` variable. The value is updated periodically
 * by other components or processes, ensuring low-latency access to temperature
 * data without performing a new scan.
 *
 * Preconditions:
 * - The `scanTemperature` variable must contain an updated temperature reading.
 *
 * Postconditions:
 * - Returns the cached CPU temperature value as a floating-point number.
 *
 * @return The cached value of the CPU temperature in degrees Celsius.
 */
float DeviceHandler::getCPUTemperatureCached()
{
    return scanTemperature;
}

/**
 * Retrieves the cached value of the water level measurement.
 *
 * This method returns the most recently stored value of the water level percentage,
 * which was previously measured and cached. The cached value provides quick access
 * without requiring a new measurement or sensor scan.
 *
 * Preconditions:
 * - The `scanWaterLevel` variable must hold a valid cached water level percentage, as
 *   updated by the appropriate scanning or sensing process.
 *
 * Postconditions:
 * - The cached water level percentage remains unchanged after this method call.
 *
 * Behavior:
 * - The method simply fetches and returns the current cached value of water level
 *   from the `scanWaterLevel` variable without initiating any additional sensor reads.
 *
 * @return The cached water level percentage as a floating-point value.
 */
float DeviceHandler::getLevelCached()
{
    return scanWaterLevel;
}

/**
 * Retrieves the most recently cached current value measured by the device.
 *
 * This method returns the current measurement previously scanned and stored
 * in the internal cache. It provides quick access to the last recorded value
 * without initiating a new sensor reading.
 *
 * Preconditions:
 * - The `scanCurrent` variable must have been updated with a valid measurement
 *   from a prior sensor scan operation.
 *
 * Postconditions:
 * - No sensor is queried or updated as this method only fetches the cached value.
 *
 * Behavior:
 * - The method simply returns the value of the `scanCurrent` variable, representing
 *   the last recorded current measurement in floating-point format.
 *
 * @return The cached current measurement as a float value.
 */
float DeviceHandler::getCurrentCached()
{
    return scanCurrent;
}

/**
 * Retrieves the most recently cached ADC voltage value.
 *
 * This method provides the last stored analog-to-digital converter (ADC)
 * voltage reading without initiating a new measurement. It serves as a
 * faster alternative to obtaining the latest ADC value when repeated
 * readings are unnecessary or when the cached value suffices.
 *
 * Preconditions:
 * - The `latestVoltage` variable must have been updated previously
 *   through a measurement or initialization process.
 *
 * Postconditions:
 * - The returned value reflects the last stored ADC voltage reading,
 *   which may not represent the current state of the ADC.
 *
 * Behavior:
 * - Always returns the cached ADC voltage value stored in `latestVoltage`.
 *
 * @return The cached ADC voltage value as a floating-point number.
 */
float DeviceHandler::getADCValueCached()
{
    return latestVoltage;
}

/**
 * Rounds a floating-point number to two decimal places.
 *
 * This method takes a single float input value and rounds it to two decimal places
 * using standard rounding logic. The result is achieved by scaling the number,
 * rounding it, and then scaling back to the original range.
 *
 * @param value The floating-point number to be rounded.
 * @return The input value rounded to two decimal places.
 */
float DeviceHandler::roundToTwoDecimals(float value)
{
    return std::round(value * 100.0f) / 100.0f;
}
