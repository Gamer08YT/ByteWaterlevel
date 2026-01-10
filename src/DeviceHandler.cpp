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
