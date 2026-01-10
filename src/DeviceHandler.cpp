//
// Created by JanHe on 10.01.2026.
//

#include "DeviceHandler.h"
#include <Arduino.h>

#include "InternalConfig.h"

// Store last Blink Timestamp.
unsigned long previousMillis = 0;

// Current LED state
bool ledState = LOW;

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
    }
    else if (relais == 2)
    {
        pin = RELAIS_CH2;
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
    pinMode(LED_PIN, OUTPUT);
    pinMode(RELAIS_CH1, OUTPUT);
    pinMode(RELAIS_CH2, OUTPUT);
}
