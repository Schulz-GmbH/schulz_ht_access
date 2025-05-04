#ifndef WEBSOCKET_MANAGER_H
#define WEBSOCKET_MANAGER_H

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "WsEvents.h"

/**
 * @class WebSocketManager
 * @brief Verwaltet den WebSocket-Server inklusive Ereignisverarbeitung und Weiterleitung.
 *
 * Diese Klasse kapselt die Einrichtung und Verwaltung einer `AsyncWebSocket`-Instanz.
 * Der WebSocket wird am HTTP-Server registriert und verarbeitet eingehende Nachrichten.
 * Dabei erfolgt die Weiterleitung der geparsten Nachrichten (ParsedMessage) an zuständige Handler.
 *
 * Unterstützte Events:
 * - `WS_EVT_CONNECT`
 * - `WS_EVT_DISCONNECT`
 * - `WS_EVT_ERROR`
 * - `WS_EVT_PONG`
 * - `WS_EVT_DATA`
 */
class WebSocketManager {
   public:
	/**
	 * @brief Konstruktor zur Initialisierung des WebSocket-Managers.
	 *
	 * @param path Der URL-Pfad des WebSocket-Endpunkts (z. B. "/ws").
	 */
	explicit WebSocketManager(const String &path = "/ws");

	/**
	 * @brief Initialisiert den WebSocket und registriert ihn am HTTP-Server.
	 *
	 * Diese Methode muss nach Erstellung des HTTP-Servers aufgerufen werden.
	 *
	 * @param httpServer Referenz auf die bestehende AsyncWebServer-Instanz.
	 */
	void init(AsyncWebServer &httpServer);

	/**
	 * @brief Gibt eine Referenz auf die WebSocket-Instanz zurück.
	 *
	 * Nützlich für Broadcasts oder externe Kommunikation (z. B. SerialBridge).
	 *
	 * @return Referenz auf den AsyncWebSocket.
	 */
	AsyncWebSocket &getSocket();

   private:
	AsyncWebSocket ws;  ///< Interne WebSocket-Instanz.

	/**
	 * @brief Statischer Wrapper für den Ereignis-Callback.
	 *
	 * Diese Methode wird als Event-Handler beim WebSocket registriert
	 * und leitet Ereignisse intern an `handleEvent()` weiter.
	 *
	 * @param server WebSocket-Instanz
	 * @param client Aktueller Client
	 * @param type Ereignistyp
	 * @param arg Zusätzliche Daten
	 * @param data Empfangene Daten
	 * @param len Länge der empfangenen Daten
	 */
	static void _onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

	/**
	 * @brief Behandelt WebSocket-Ereignisse und verteilt sie weiter.
	 *
	 * Interne Weiterleitung der Ereignisse, insbesondere des `DATA`-Events,
	 * an spezialisierte Handler.
	 *
	 * @param client Referenz auf den WebSocket-Client.
	 * @param type Typ des Ereignisses (CONNECT, DISCONNECT, DATA, ...).
	 * @param arg Weitere Daten.
	 * @param data Empfangene Daten.
	 * @param len Länge der empfangenen Daten.
	 */
	void handleEvent(AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

	/**
	 * @brief Leitet geparste Nachrichten (ParsedMessage) an zuständige Handler weiter.
	 *
	 * Diese Methode wird nur bei `WS_EVT_DATA` aufgerufen und wertet
	 * das `eventType`-Feld des ParsedMessage-Objekts aus.
	 *
	 * @param client Referenz auf den Client, von dem die Nachricht stammt.
	 * @param msg Geparste Nachricht vom Typ ParsedMessage.
	 */
	void onData(AsyncWebSocketClient *client, ParsedMessage &msg);
};

#endif  // WEBSOCKET_MANAGER_H
