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
    // 'true' formatiert das Dateisystem, falls es noch nicht existiert
    if(!LittleFS.begin(true)){
        Serial.println("Fehler beim Mounten von LittleFS");
        return;
    }

    // 2. WLAN Verbindung herstellen
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Verbinde mit WLAN");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nVerbunden!");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());

    // 3. WebSocket Route initialisieren
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // 4. WebServer Route: index.html aus dem Dateisystem laden
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        // Prüfen, ob die Datei existiert
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
}