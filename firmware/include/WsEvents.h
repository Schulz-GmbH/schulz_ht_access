/**
 * @file WsEvents.h
 * @brief Header-Datei für die WebSocket-Ereignisbehandlung und -verarbeitung.
 *
 * Dieses Modul definiert die Struktur und Hilfsfunktionen zur Verarbeitung eingehender
 * WebSocket-Nachrichten im JSON-Format sowie zur Ausführung entsprechender System-, Log- oder Serial-Kommandos.
 * Zusätzlich stellt es Funktionen zum Antworten, Parsen und Bearbeiten von Nachrichten zur Verfügung.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#ifndef WS_EVENTS_H
#define WS_EVENTS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "LLog.h"
#include "WiFiManager.h"

/**
 * @brief WebSocket-Ereignistypen.
 *
 * Definiert, welche Art von Nachricht über den WebSocket empfangen wurde.
 */
enum WsEvents {
	WS_EVT_SYSTEM,  ///< Systembezogenes Ereignis (z. B. Netzwerkscan, Reboot)
	WS_EVT_LOG,     ///< Ereignis zur Protokollierung oder Logdateiverwaltung
	WS_EVT_SERIAL   ///< Ereignis zur Kommunikation mit seriellen Schnittstellen
};

/**
 * @brief Struktur für geparste WebSocket-Nachrichten.
 *
 * Diese Struktur enthält die zerlegte Form einer empfangenen JSON-WebSocket-Nachricht.
 */
struct ParsedMessage {
	WsEvents eventType;  ///< Typ des WebSocket-Ereignisses
	String command;      ///< Kommando (z. B. "scan", "get", "delete")
	String key;          ///< Schlüssel oder Identifikator für das Kommando
	String value;        ///< Wert (optional) zur Verarbeitung des Kommandos
};

/**
 * @brief Parst eine empfangene WebSocket-Nachricht im JSON-Format.
 *
 * @param jsonData Die empfangene Nachricht im JSON-Format.
 * @return ParsedMessage Struktur mit interpretierten Werten.
 */
ParsedMessage parseWebSocketMessage(const char *jsonData);

/**
 * @brief Behandelt systembezogene WebSocket-Nachrichten.
 *
 * @param client Der WebSocket-Client, der die Nachricht gesendet hat.
 * @param msg Die geparste Nachricht.
 */
void handleSystemEvent(AsyncWebSocketClient *client, const ParsedMessage &msg);

/**
 * @brief Behandelt Log-spezifische WebSocket-Nachrichten.
 *
 * @param client Der WebSocket-Client, der die Nachricht gesendet hat.
 * @param msg Die geparste Nachricht.
 */
void handleLogEvent(AsyncWebSocketClient *client, const ParsedMessage &msg);

/**
 * @brief Behandelt Nachrichten zur seriellen Kommunikation.
 *
 * @param client Der WebSocket-Client, der die Nachricht gesendet hat.
 * @param msg Die geparste Nachricht.
 */
void handleSerialEvent(AsyncWebSocketClient *client, const ParsedMessage &msg);

/**
 * @brief Sendet eine standardisierte Antwort an den WebSocket-Client.
 *
 * @param client Zielclient.
 * @param event Ereignisname.
 * @param action Ausgeführte Aktion.
 * @param status Statusmeldung (z. B. "ok", "error").
 * @param details Detailnachricht.
 * @param error Fehlertext (falls vorhanden).
 */
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const String &details,
                  const String &error);

/**
 * @brief Sendet eine Antwort mit JSON-Array als Detailinhalt.
 *
 * @param client Zielclient.
 * @param event Ereignisname.
 * @param action Ausgeführte Aktion.
 * @param status Statusmeldung (z. B. "ok", "error").
 * @param details JSON-Array mit weiteren Details.
 * @param error Fehlertext (falls vorhanden).
 */
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const JsonArray &details,
                  const String &error);

/**
 * @brief Event-Callback für den WebSocket-Server.
 *
 * Wird bei Eintreffen von Nachrichten, neuen Verbindungen oder Trennungen aufgerufen.
 *
 * @param server Referenz auf den WebSocket-Server.
 * @param client WebSocket-Client, der das Ereignis ausgelöst hat.
 * @param type Art des Ereignisses (z. B. Verbindung, Nachricht).
 * @param arg Zusatzdaten je nach Ereignistyp.
 * @param data Empfangene Daten (bei Nachrichten).
 * @param len Länge der empfangenen Daten.
 */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

/**
 * @brief Task zum asynchronen Scannen von WLAN-Netzwerken.
 *
 * @param parameter Unbenutzter Parameter.
 */
void scanNetworksTask(void *parameter);

/**
 * @brief Sendet die Liste der verfügbaren Logdateien über WebSocket an den Client.
 *
 * @param client Zielclient.
 */
void listLogFiles(AsyncWebSocketClient *client);

/**
 * @brief Task zur asynchronen Ermittlung der Logdateien auf dem Dateisystem.
 *
 * @param pvParameters Unbenutzte Parameter.
 */
void listLogFilesTask(void *pvParameters);

/**
 * @brief Benennt eine Logdatei anhand übergebener JSON-Daten um.
 *
 * @param client Zielclient.
 * @param jsonData JSON-Daten mit alter und neuer Dateibezeichnung.
 */
void renameLogFile(AsyncWebSocketClient *client, const String &jsonData);

/**
 * @brief Löscht eine einzelne Logdatei anhand des Namens.
 *
 * @param client Zielclient.
 * @param fileName Dateiname der zu löschenden Logdatei.
 */
void deleteSingleLogFile(AsyncWebSocketClient *client, const String &fileName);

/**
 * @brief Löscht mehrere Logdateien anhand von JSON-Daten.
 *
 * @param client Zielclient.
 * @param jsonData JSON-Array mit Dateinamen.
 */
void deleteLogFiles(AsyncWebSocketClient *client, const String &jsonData);

#endif  // WS_EVENTS_H
