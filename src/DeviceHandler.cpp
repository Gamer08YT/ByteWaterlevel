//
// Created by JanHe on 10.01.2026.
//

#include "DeviceHandler.h"
#include <Arduino.h>

#include "InternalConfig.h"

// Store last Blink Timestamp.
unsigned long previousMillis = 0;

// Store last Relais Duration Timestamp.
unsigned long ch1 = -1;
unsigned long ch2 = -1;

// Store Relais States.
bool relay1 = LOW;
bool relay2 = LOW;


// Current LED state
bool ledState = LOW;

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

void DeviceHandler::loop()
{
    handleBlink();
    handleRelais();
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
    analogSetPinAttenuation(SENSE, ADC_6db);


    // Input Pins.
    pinMode(SENSE, INPUT);

    // Output Pins.
    pinMode(LED_PIN, OUTPUT);
    pinMode(RELAIS_CH1, OUTPUT);
    pinMode(RELAIS_CH2, OUTPUT);
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
    return readVoltage(SENSE, 256);
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
