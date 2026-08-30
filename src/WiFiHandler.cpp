//
// Created by JanHe on 09.01.2026.
//

#include "WiFiHandler.h"
#include <WiFi.h>
#include "FileHandler.h"
#include "InternalConfig.h"
#include "DeviceHandler.h"

// Konstanten
const unsigned long WiFiHandler::CONNECTION_TIMEOUT_MS = 30000; // 30 Sekunden

// Statische Variablen
unsigned long WiFiHandler::connectionStartTime = 0;
bool WiFiHandler::wasConnected = false;

bool WiFiHandler::apStarted = false;
static unsigned long lastAPStartAttempt = 0;


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
        DeviceHandler::setLedState(WIFI_CONNECTING);
        // Set Device Hostname (steal from AP Settings).
        WiFi.hostname(config["wifi"]["ap"]["ssid"].as<String>());

        // Set AP mode explicitly
        WiFi.mode(WIFI_MODE_STA);

        // Begin Wi-Fi Connection.
        WiFi.begin(config["wifi"]["client"]["ssid"].as<String>(),
                   config["wifi"]["client"]["password"].as<String>());

        // Start Timer for Connection Timeout.
        connectionStartTime = millis();
        wasConnected = false;
        apStarted = false;


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
    Serial.println("WiFi setup finished. Connection will be handled asynchronously.");
#endif
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
 * - If the system is not connected to Wi-Fi in STA mode, the Arduino WiFi
 *   driver performs reconnect attempts because auto-reconnect is enabled.
 * - If the connection timeout is exceeded, it starts an AP+STA fallback so a
 *   client can connect directly while the driver keeps trying the STA network.
 *
 * This method plays a critical role in dynamically adapting to network
 * availability while ensuring robust and continuous Wi-Fi operation.
 */
void WiFiHandler::checkConnection()
{
    const unsigned long now = millis();

    // WiFi.setAutoReconnect() owns reconnect attempts. Do not call
    // WiFi.reconnect() periodically here: doing so can restart the driver's
    // association state while it is already attempting to reconnect.
    if (isConnected())
    {
        const bool connectedNow = !wasConnected;
        wasConnected = true;
        connectionStartTime = 0;
        if (connectedNow || apStarted)
            DeviceHandler::setLedState(NORMAL);

        // Once STA is back, the fallback AP is no longer needed. Stopping the
        // AP first also prevents changing the mode from APSTA to STA from
        // unnecessarily interrupting the newly restored STA connection.
        if (apStarted)
        {
            stopAP();
            WiFi.mode(WIFI_MODE_STA);
        }

#if DEBUG == true
        if (connectedNow)
            Serial.println("WiFi connected. Auto-reconnect is active.");
#endif
        return;
    }

    // Start one timeout window for the initial connection or for a later
    // disconnect. Unsigned subtraction remains correct across millis() wrap.
    if (connectionStartTime == 0)
    {
        connectionStartTime = now;
        wasConnected = false;
        DeviceHandler::setLedState(WIFI_CONNECTING);

#if DEBUG == true
        Serial.println("WiFi disconnected. Auto-reconnect is active...");
#endif
    }

    // Keep the AP available as a configuration fallback while the ESP32
    // continues trying the configured STA network in APSTA mode.
    if (!apStarted && now - connectionStartTime >= CONNECTION_TIMEOUT_MS &&
        now - lastAPStartAttempt >= 5000)
    {
#if DEBUG == true
        Serial.println("WiFi timeout. Starting fallback AP...");
#endif

        JsonDocument config = FileHandler::getConfig();

        lastAPStartAttempt = now;
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
    const String ssid = config["wifi"]["ap"]["ssid"].as<String>();
    const String password = config["wifi"]["ap"]["password"].as<String>();

    // softAP() fails for an empty SSID or an invalid WPA password. Avoid
    // repeatedly invoking the driver with a corrupt configuration.
    if (ssid.isEmpty() || (!password.isEmpty() && password.length() < 8))
    {
        Serial.println("AP configuration invalid: SSID/password missing or password too short.");
        return;
    }

    DeviceHandler::setLedState(AP_MODE);
    // Wechsel zum AP-Modus
    WiFi.mode((combine ? WIFI_MODE_APSTA : WIFI_MODE_AP));

    // Define Subnet before beginning Soft AP.
    WiFi.softAPConfig(apIP, gateway, subnet);

    // Begin Soft AP.
    bool started = WiFi.softAP(ssid, password);

    // Set the flag only if the AP really started. This allows a later loop
    // iteration to retry after a transient driver failure.
    if (started)
        apStarted = true;
    else
        Serial.println("AP could not be started.");

#if DEBUG == true
    Serial.print("AP startet: ");
    Serial.println(ssid);
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
