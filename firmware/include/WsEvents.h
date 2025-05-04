/**
 * @file WsEvents.h
 * @brief Schnittstelle für die Verarbeitung und das Routing von WebSocket-Ereignissen.
 *
 * Dieses Modul definiert die notwendigen Typen, Hilfsfunktionen und Handler zur Verarbeitung
 * eingehender WebSocket-Nachrichten im JSON-Format. Je nach Ereignistyp (`system`, `log`, `serial`)
 * werden die Nachrichten an entsprechende Handler-Funktionen weitergeleitet.
 */

#ifndef WSEVENTS_H
#define WSEVENTS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "LLog.h"
#include "WiFiManager.h"

/// Globale Instanz des WiFiManagers
extern WiFiManager wifiManager;  ///< globale Instanz

/**
 * @enum WsEvents
 * @brief Repräsentiert die unterstützten WebSocket-Ereignistypen.
 */
enum WsEvents { WS_EVT_SYSTEM, WS_EVT_LOG, WS_EVT_SERIAL };

/**
 * @struct ParsedMessage
 * @brief Struktur zur Darstellung einer geparsten WebSocket-Nachricht.
 */
struct ParsedMessage {
	WsEvents eventType;
	String command;
	String key;
	String value;
};

/**
 * @brief Wandelt einen Typ-String (z. B. "system") in ein `WsEvents`-Enum.
 *
 * @param type String mit dem Typnamen.
 * @return Passendes `WsEvents`-Enum (Default: `WS_EVT_SYSTEM`).
 */
WsEvents getEventType(const String &type);

/**
 * @brief Parst eine JSON-WebSocket-Nachricht in eine `ParsedMessage`.
 *
 * @param jsonData Die Rohdaten im JSON-Format.
 * @return Eine geparste Nachricht mit Event-Typ, Befehl, Schlüssel und Wert.
 */
ParsedMessage parseWebSocketMessage(const char *jsonData);

/**
 * @deprecated
 * @brief Haupt-Eventhandler für WebSocket-Ereignisse.
 *
 * Wird typischerweise vom WebSocketManager registriert.
 *
 * @param server WebSocket-Serverinstanz.
 * @param client Der Client, der das Ereignis auslöste.
 * @param type Art des Ereignisses (z. B. CONNECT, DATA, DISCONNECT).
 * @param arg Weitere Parameter (kontextabhängig).
 * @param data Empfangene Daten (z. B. JSON).
 * @param len Länge der empfangenen Daten.
 */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

/*
 * -------------------------------------------------------------------------------------------------
 * Action-Handler
 *-------------------------------------------------------------------------------------------------
 */

/**
 * @brief Behandelt eingehende "system"-Nachrichten (WLAN etc.).
 *
 * @param client Der WebSocket-Client.
 * @param msg Die geparste Nachricht.
 */
void handleSystemEvent(AsyncWebSocketClient *client, const ParsedMessage &msg);

/**
 * @brief Behandelt eingehende "log"-Nachrichten (Logging-Funktionen).
 *
 * @param client Der WebSocket-Client.
 * @param msg Die geparste Nachricht.
 */
void handleLogEvent(AsyncWebSocketClient *client, const ParsedMessage &msg);

/**
 * @brief Behandelt eingehende "serial"-Nachrichten (serielle Kommunikation).
 *
 * @param client Der WebSocket-Client.
 * @param msg Die geparste Nachricht.
 */
void handleSerialEvent(AsyncWebSocketClient *client, const ParsedMessage &msg);

/*
 * -------------------------------------------------------------------------------------------------
 * Hilfs-Tasks
 *-------------------------------------------------------------------------------------------------
 */

/**
 * @brief FreeRTOS-Task zum asynchronen Scannen nach WLAN-Netzwerken.
 *
 * @param parameter Pointer auf den aufrufenden WebSocketClient.
 */
void scanNetworksTask(void *parameter);

/**
 * @brief FreeRTOS-Task zur asynchronen Verbindung mit einem WLAN.
 *
 * @param parameter Struktur mit WebSocketClient, SSID und Passwort.
 */
void connectNetworkTask(void *parameter);

/**
 * @brief Sendet den aktuellen Status des Datei-Loggings zurück.
 *
 * @param client Ziel-Client.
 */
void sendDebugResponse(AsyncWebSocketClient *client);

/*
 * -------------------------------------------------------------------------------------------------
 * Response-Helper
 *-------------------------------------------------------------------------------------------------
 */

/**
 * @brief Sendet eine Antwort über WebSocket mit `details` als String.
 *
 * @param client Ziel-Client.
 * @param event Ereignis-Typ.
 * @param action Aktion oder Kontext.
 * @param status Status-String ("success", "error", ...).
 * @param details Detailinformationen (als einfacher String).
 * @param error Optionaler Fehlertext.
 */
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const String &details,
                  const String &error = "");

/**
 * @deprecated
 * @brief (veraltet) Sendet eine WebSocket-Antwort mit einem JSON-Array als `details`.
 *
 * @param client Ziel-Client.
 * @param event Ereignis-Typ.
 * @param action Aktion.
 * @param status Status.
 * @param details Array mit Informationen.
 * @param error Optionaler Fehlertext.
 */
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const JsonArray &details,
                  const String &error = "");

/**
 * @brief Sendet eine Antwort über WebSocket mit beliebigem JSON-Wert als Details.
 *
 * @param client Ziel-Client.
 * @param event Ereignis-Typ.
 * @param action Aktion.
 * @param status Status (z. B. "success" oder "error").
 * @param details Beliebiger JSON-Wert (Objekt, Array, String, ...).
 */
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const JsonVariantConst &details);

#endif  // WSEVENTS_H
