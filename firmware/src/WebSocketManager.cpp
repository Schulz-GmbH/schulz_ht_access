/**
 * @file WebSocketManager.cpp
 * @brief Verwaltung der WebSocket-Kommunikation über eine AsyncWebSocket-Instanz.
 *
 * Dieses Modul kapselt die Initialisierung, Verwaltung und Ereignisbehandlung eines WebSocket-Servers.
 * Die Kommunikation erfolgt JSON-basiert, empfangene Nachrichten werden analysiert (geparst) und
 * an zuständige Handler weitergeleitet.
 *
 * Unterstützte Ereignistypen:
 * - WS_EVT_CONNECT: Verbindungsaufbau
 * - WS_EVT_DISCONNECT: Verbindungsabbruch
 * - WS_EVT_ERROR: Fehler beim Client
 * - WS_EVT_PONG: Antwort auf Ping
 * - WS_EVT_DATA: Empfangene Daten (werden an spezialisierte Event-Handler übergeben)
 *
 * Die WebSocketManager-Klasse übernimmt die Weiterleitung an:
 * - System-Events (handleSystemEvent)
 * - Log-Events (handleLogEvent)
 * - Serial-Events (handleSerialEvent)
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#include "WebSocketManager.h"

#include "SerialBridge.h"
#include "WsEvents.h"
#include "global.h"

extern SerialBridge *serialBridge;
/**
 * @brief Konstruktor für WebSocketManager.
 *
 * Initialisiert eine AsyncWebSocket-Instanz mit dem übergebenen Pfad.
 *
 * @param path WebSocket-Endpunkt, z. B. "/ws"
 */
WebSocketManager::WebSocketManager(const String &path) : ws(path) {
}

/**
 * @brief Initialisiert den WebSocket-Server.
 *
 * Registriert den WebSocket beim HTTP-Server und bindet das Event-Callback.
 *
 * @param httpServer Referenz auf den bereits bestehenden AsyncWebServer.
 */
void WebSocketManager::init(AsyncWebServer &httpServer) {
	// WebSocket als Handler registrieren
	httpServer.addHandler(&ws);

	// Statischer Callback ruft Member-Methoden auf
	ws.onEvent(_onEvent);
}

/**
 * @brief Gibt eine Referenz auf den internen WebSocket zurück.
 *
 * Ermöglicht Zugriff auf den WebSocket für Broadcasts o. ä.
 *
 * @return Referenz auf AsyncWebSocket.
 */
AsyncWebSocket &WebSocketManager::getSocket() {
	return ws;
}

/**
 * @brief Statischer Callback-Wrapper zur Weiterleitung auf handleEvent().
 *
 * Wird intern von `ws.onEvent(...)` aufgerufen.
 *
 * @param server Referenz auf die WebSocket-Instanz.
 * @param client Verbindener WebSocket-Client.
 * @param type Ereignistyp.
 * @param arg Zusatzdaten.
 * @param data Empfangene Daten.
 * @param len Länge der empfangenen Daten.
 */
void WebSocketManager::_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	// ws->getUserData() oder globalen Pointer nutzen
	// Da wir nur eine Instanz haben, rufen wir globalen Manager auf:
	extern WebSocketManager webSocketManager;
	webSocketManager.handleEvent(client, type, arg, data, len);
}

/**
 * @brief Behandelt WebSocket-Ereignisse für einen bestimmten Client.
 *
 * Je nach Ereignistyp werden Logs geschrieben oder – bei `WS_EVT_DATA` –
 * die Nachricht geparst und an `onData()` weitergeleitet.
 *
 * @param client Referenz auf den WebSocket-Client.
 * @param type Typ des Ereignisses.
 * @param arg Zusatzdaten.
 * @param data Empfangene Daten.
 * @param len Länge der Daten.
 */
void WebSocketManager::handleEvent(AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	switch (type) {
		case WS_EVT_CONNECT:
			logger.log({"socket", "info"}, "WS Client connected: " + String(client->id()));
			if (serialBridge) serialBridge->sendAvailability();
			break;
		case WS_EVT_DISCONNECT:
			logger.log({"socket", "info"}, "WS Client disconnected: " + String(client->id()));
			break;
		case WS_EVT_ERROR:
			logger.log({"socket", "error"}, "WS Error on client " + String(client->id()));
			break;
		case WS_EVT_PONG:
			logger.log({"socket", "info"}, "WS Pong from client " + String(client->id()));
			break;
		case WS_EVT_DATA: {
			// parse und dispatch
			ParsedMessage msg = parseWebSocketMessage((char *)data);
			onData(client, msg);
			break;
		}
		default:
			break;
	}
}

/**
 * @brief Verarbeitet eingehende Nutzdaten (DATA) und dispatcht sie an spezialisierte Handler.
 *
 * Die Methode wird nur bei `WS_EVT_DATA` aufgerufen. Die Nachricht wurde bereits in ein
 * ParsedMessage-Objekt umgewandelt.
 *
 * @param client Referenz auf den WebSocket-Client.
 * @param msg Geparste Nachricht.
 */
void WebSocketManager::onData(AsyncWebSocketClient *client, ParsedMessage &msg) {
	// hier nur dispatchen, wie vorher in WsEvents.cpp:
	switch (msg.eventType) {
		case WS_EVT_SYSTEM:
			handleSystemEvent(client, msg);
			break;
		case WS_EVT_LOG:
			handleLogEvent(client, msg);
			break;
		case WS_EVT_SERIAL:
			handleSerialEvent(client, msg);
			break;
	}
}
