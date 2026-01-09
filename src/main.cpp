/**
 * Wochenendprojekt ;)
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

// WLAN Zugangsdaten - Bitte anpassen
const char* ssid = "DEIN_WLAN_NAME";
const char* password = "DEIN_WLAN_PASSWORT";

// AP Zugangsdaten (Fallback)
const char* ap_ssid = "ByteWaterlevel-AP";

// Zeit-Management für Reconnect
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 30000; // 30 Sekunden

// Server auf Port 80 und WebSocket auf /ws
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// WebSocket Event Handler
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket Client #%u verbunden von %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket Client #%u getrennt\n", client->id());
            break;
        case WS_EVT_DATA:
            // Beispiel: Textnachrichten verarbeiten
            // handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void setup() {
    Serial.begin(115200);

    // 1. Dateisystem (LittleFS) starten
    if(!LittleFS.begin(true)){
        Serial.println("Fehler beim Mounten von LittleFS");
        return;
    }

    // 2. WLAN Verbindung herstellen (mit Timeout Logik)
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Verbinde mit WLAN");

    unsigned long startAttemptTime = millis();
    bool connected = false;

    // Versuche 15 Sekunden lang zu verbinden
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
        Serial.print(".");
        delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nVerbunden!");
        Serial.print("IP-Adresse: ");
        Serial.println(WiFi.localIP());
        connected = true;
    } else {
        Serial.println("\nKeine Verbindung nach 15s. Starte AP Modus...");
        // Wechsel zu AP + STA, damit wir später im Hintergrund weiter versuchen können zu verbinden
        WiFi.mode(WIFI_AP_STA);
        // AP starten (offen oder mit Passwort, hier offen für einfacheres Debugging)
        WiFi.softAP(ap_ssid);
        Serial.print("AP IP-Adresse: ");
        Serial.println(WiFi.softAPIP());
    }

    // 3. WebSocket Route initialisieren
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // 4. WebServer Route: index.html aus dem Dateisystem laden
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        if (LittleFS.exists("/index.html")) {
            request->send(LittleFS, "/index.html", "text/html");
        } else {
            request->send(404, "text/plain", "File not found. Did you upload the filesystem image?");
        }
    });

    // 5. Server starten
    server.begin();
}

void loop() {
    // Wichtig für AsyncWebSocket Garbage Collection
    ws.cleanupClients();

    // Reconnect Logik: Alle 30 Sekunden prüfen, wenn nicht verbunden
    unsigned long currentMillis = millis();
    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - lastReconnectAttempt >= reconnectInterval)) {
        Serial.println("WLAN Verbindung verloren oder nicht vorhanden. Versuche Reconnect...");
        // Wichtig: WiFi.disconnect() nicht aufrufen, da sonst der AP Modus gestört werden könnte
        WiFi.begin(ssid, password);
        lastReconnectAttempt = currentMillis;
    }
}