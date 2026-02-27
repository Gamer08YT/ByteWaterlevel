//
// Created by JanHe on 09.01.2026.
//

#include "WiFiHandler.h"
#include <WiFi.h>
#include "FileHandler.h"
#include "InternalConfig.h"

// Konstanten
const unsigned long WiFiHandler::CONNECTION_TIMEOUT_MS = 30000; // 30 Sekunden

// Statische Variablen
unsigned long WiFiHandler::connectionStartTime = 0;

// Store last Reconnect Attempt Timestamp.
long lastReconnectAttempt;

bool WiFiHandler::apStarted = false;


// I use an UniFi Network on 192.XXX.XXX.XXX so i will use an 10.XX.XX.XX for Debugging.
IPAddress apIP(10, 10, 10, 1);
IPAddress gateway(10, 10, 10, 1);
IPAddress subnet(255, 255, 255, 0);

void WiFiHandler::setup()
{
    // Enable Auto Reconnect.
    WiFi.setAutoReconnect(true);

    JsonDocument config = FileHandler::getConfig();

    // Check if Wi-Fi Credentials are set.
    if (isWiFiClientUsable())
    {
        // Set Device Hostname (steal from AP Settings).
        WiFi.hostname(config["wifi"]["ap"]["ssid"].as<String>());

        // Set AP mode explicitly
        WiFi.mode(WIFI_MODE_STA);

        // Begin Wi-Fi Connection.
        WiFi.begin(config["wifi"]["client"]["ssid"].as<String>(),
                   config["wifi"]["client"]["password"].as<String>());

        // Start Timer for Connection Timeout.
        connectionStartTime = millis();
        apStarted = false;


        // Wait for Connection.
        int result = WiFi.waitForConnectResult(15000);

        if (result != WL_CONNECTED)
        {
            startAP(config, false);
        }

#if DEBUG == true
        Serial.println("WiFi Client started. Try to connect...");
#endif
    }
    else
    {
        // Invalid Credentials, start AP.
        startAP(config, false);
    }

#if DEBUG == true
    Serial.println("WiFi started");
#endif

    Serial.print("Ready on ");
    Serial.println(WiFi.softAPIP());
}

/**
 * Executes the primary logic of the WiFiHandler in a continuous loop. This
 * method is typically called repeatedly in the main application loop. Within its
 * execution, it ensures the Wi-Fi connection is maintained by invoking the
 * checkConnection method. The method is responsible for managing the current
 * Wi-Fi state and handling reconnection or Access Point (AP) initiation if
 * necessary.
 *
 * This function facilitates the seamless operation of Wi-Fi capability as part
 * of the application's larger loop structure.
 */
void WiFiHandler::loop()
{
    checkConnection();
}

/**
 * Determines if the device is currently connected to a Wi-Fi network. This
 * method evaluates the Wi-Fi connection status and returns a boolean indicating
 * whether the connection is active.
 *
 * @return true if the Wi-Fi is connected (status is WL_CONNECTED), false otherwise.
 */
bool WiFiHandler::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

/**
 * Retrieves the current Received Signal Strength Indicator (RSSI) value
 * of the connected Wi-Fi network. RSSI represents the signal strength,
 * where higher negative values indicate a weaker signal (e.g., -30 dBm
 * represents a strong signal, while -90 dBm represents a weak signal).
 *
 * @return The current RSSI value as a float, measured in dBm.
 */
float WiFiHandler::getRSSI()
{
    return WiFi.RSSI();
}


/**
 * Monitors and maintains the Wi-Fi connection state. The method ensures that the
 * system is always capable of network communication by handling transitions
 * between Station (STA) and Access Point (AP) modes, depending on the connection
 * status and elapsed time since a disconnection occurred.
 *
 * - If the system is connected in STA mode while the AP mode is active, the
 *   method stops the AP to ensure only STA mode is used.
 * - If the system is not connected to Wi-Fi in STA mode:
 *     - Tries to reconnect to the Wi-Fi network if no reconnection is ongoing.
 *     - If the reconnect attempt exceeds a predefined timeout, it switches the
 *       system to AP mode, thereby allowing a client to connect directly to the
 *       device for further configuration or operation.
 *
 * This method plays a critical role in dynamically adapting to network
 * availability while ensuring robust and continuous Wi-Fi operation.
 */
void WiFiHandler::checkConnection()
{
    // If STA is connected and AP is active → stop AP
    if (isConnected() && apStarted)
    {
        // Stop AP Mode.
        stopAP();

        // Return to clean STA mode
        WiFi.mode(WIFI_MODE_STA);

#if DEBUG == true
        Serial.println("STA reconnected. AP stopped.");
#endif
        return;
    }

    // If STA is connected → nothing to do
    if (isConnected())
        return;

    // STA is NOT connected → handle reconnect logic
    unsigned long now = millis();

    // Try to reconnect every 5 seconds
    if (now - lastReconnectAttempt >= 5000)
    {
        lastReconnectAttempt = now;

#if DEBUG == true
        Serial.println("WiFi lost. Trying reconnect...");
#endif

        WiFi.reconnect();
    }

    // If reconnect takes too long → start AP
    if (!apStarted && now - connectionStartTime > CONNECTION_TIMEOUT_MS)
    {
#if DEBUG == true
        Serial.println("Reconnect timeout. Starting AP...");
#endif

        JsonDocument config = FileHandler::getConfig();

        // AP+STA mode
        startAP(config, true);
    }
}


/**
 * Initializes and starts a Wi-Fi Access Point (AP) using the provided configuration.
 * The method sets the Wi-Fi mode to AP, configures the subnet, and starts the AP
 * with the SSID and password extracted from the provided configuration object.
 *
 * @param config The JsonDocument that contains the configuration data for the AP,
 *               including the SSID and password under "wifi.ap".
 * @param combine
 */
void WiFiHandler::startAP(JsonDocument& config, bool combine)
{
    // Wechsel zum AP-Modus
    WiFi.mode((combine ? WIFI_MODE_APSTA : WIFI_MODE_AP));

    // Define Subnet before beginning Soft AP.
    WiFi.softAPConfig(apIP, gateway, subnet);

    // Begin Soft AP.
    WiFi.softAP(config["wifi"]["ap"]["ssid"].as<String>(),
                config["wifi"]["ap"]["password"].as<String>());

    apStarted = true;

#if DEBUG == true
    Serial.print("AP startet: ");
    Serial.println(config["wifi"]["ap"]["ssid"].as<String>());
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
#endif
}

/**
 * Checks if the Wi-Fi client configuration is valid and usable.
 * The method verifies whether the necessary Wi-Fi credentials
 * (SSID and password) are present in the configuration file.
 *
 * @return true if the Wi-Fi configuration contains valid credentials;
 *         false otherwise.
 */
bool WiFiHandler::isWiFiClientUsable()
{
    JsonDocument config = FileHandler::getConfig();

    return (!config["wifi"].isNull() &&
        !config["wifi"]["client"]["ssid"].isNull() &&
        !config["wifi"]["client"]["ssid"].as<String>().isEmpty() &&
        !config["wifi"]["client"]["password"].isNull());
}

/**
 * Stops the Wi-Fi Access Point (AP) mode if it is currently active.
 *
 * This method checks if the AP has been started by verifying the `apStarted`
 * flag. If the AP is active, it disconnects the Access Point using the
 * WiFi.softAPdisconnect method and updates the `apStarted` flag to false.
 * It ensures that the device exits AP mode cleanly.
 */
void WiFiHandler::stopAP()
{
    if (apStarted)
    {
        WiFi.softAPdisconnect(true);
        apStarted = false;

#if DEBUG == true
        Serial.println("AP stopped.");
#endif
    }
}
