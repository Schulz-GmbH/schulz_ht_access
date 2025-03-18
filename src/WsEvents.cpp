/**
 * @file WsEvents.cpp
 * @brief Modul zur Verwaltung und Verarbeitung von WebSocket-Ereignissen.
 *
 * Dieses Modul implementiert Funktionen zum Parsen eingehender WebSocket-Nachrichten
 * im JSON-Format, ermittelt deren Ereignistyp und leitet sie zur weiteren Verarbeitung
 * an spezialisierte Event-Handler (System, Log, Serial) weiter.
 *
 * Zusätzlich werden Hilfsfunktionen bereitgestellt, um JSON-basierte Antworten über
 * WebSocket an die verbundenen Clients zu senden.
 *
 * Unterstützte Ereignistypen sind:
 * - WS_EVT_SYSTEM: Systembezogene Ereignisse.
 * - WS_EVT_LOG: Log-bezogene Ereignisse.
 * - WS_EVT_SERIAL: Serielle Kommunikation betreffende Ereignisse.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#include "WsEvents.h"

/**
 * @brief Bestimmt den WebSocket-Ereignistyp aus einem String.
 *
 * @param type String, der den Ereignistyp repräsentiert.
 * @return Der passende WsEvents-Enum-Wert.
 */
WsEvents getEventType(const String &type) {
	if (type == "system") return WS_EVT_SYSTEM;
	if (type == "log") return WS_EVT_LOG;
	if (type == "serial") return WS_EVT_SERIAL;
	return WS_EVT_SYSTEM;  // Fallback auf SYSTEM
}

/**
 * @brief Parst eine WebSocket-Nachricht im JSON-Format in eine ParsedMessage-Struktur.
 *
 * @param jsonData Eingehende JSON-formatierte Nachricht.
 * @return ParsedMessage mit extrahierten Daten.
 */
ParsedMessage parseWebSocketMessage(const char *jsonData) {
	ParsedMessage msg;
	StaticJsonDocument<256> doc;
	DeserializationError error = deserializeJson(doc, jsonData);

	if (!error) {
		String type = doc["type"].as<String>();
		msg.eventType = getEventType(type);
		msg.command = doc["command"].as<String>();
		msg.key = doc["key"].as<String>();
		msg.value = doc["value"].as<String>();
	}
	return msg;
}

/**
 * @brief WebSocket Event-Handler für Verbindung, Datenempfang, Trennung und Fehler.
 *
 * @param server WebSocket-Server-Instanz.
 * @param client WebSocket-Client, der das Ereignis ausgelöst hat.
 * @param type Ereignistyp (Verbindung, Trennung, Datenempfang, Fehler).
 * @param arg Zusätzliche Argumente (nicht genutzt).
 * @param data Empfangene Daten.
 * @param len Länge der empfangenen Daten.
 */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	switch (type) {
		case WS_EVT_CONNECT:
			Serial.printf("WebSocket-Client %u verbunden.\n", client->id());
			break;

		case WS_EVT_DISCONNECT:
			Serial.printf("WebSocket-Client %u getrennt.\n", client->id());
			break;

		case WS_EVT_ERROR:
			Serial.printf("WebSocket-Fehler bei Client %u.\n", client->id());
			break;

		case WS_EVT_PONG:
			Serial.printf("WebSocket-Ping/Pong von Client %u.\n", client->id());
			break;

		case WS_EVT_DATA: {
			ParsedMessage msg = parseWebSocketMessage((char *)data);
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
				default:
					logger.warn("Unbekannter WebSocket-Event im DATA-Payload!");
			}
			break;
		}
		default:
			logger.warn("Unbekannter WebSocket-Event!");
			break;
	}
}

/**
 * @brief Behandelt "system"-Ereignisse vom WebSocket.
 *
 * @param client Client, der das Ereignis gesendet hat.
 * @param msg Die empfangene Nachricht als ParsedMessage.
 */
void handleSystemEvent(AsyncWebSocketClient *client, const ParsedMessage &msg) {
	logger.info("handleSystemEvent: command=" + msg.command + ", key=" + msg.key + ", value=" + msg.value);
	// Weitere Verarbeitung, z.B. Senden einer Antwort:
	sendResponse(client, "system", "response", "success", "System Event verarbeitet", "");
}

/**
 * @brief Behandelt "log"-Ereignisse vom WebSocket.
 *
 * @param client Client, der das Ereignis gesendet hat.
 * @param msg Die empfangene Nachricht als ParsedMessage.
 */
void handleLogEvent(AsyncWebSocketClient *client, const ParsedMessage &msg) {
	logger.info("handleLogEvent: command=" + msg.command + ", key=" + msg.key + ", value=" + msg.value);
	sendResponse(client, "log", "response", "success", "Log Event verarbeitet", "");
}

/**
 * @brief Behandelt "serial"-Ereignisse vom WebSocket.
 *
 * @param client Client, der das Ereignis gesendet hat.
 * @param msg Die empfangene Nachricht als ParsedMessage.
 */
void handleSerialEvent(AsyncWebSocketClient *client, const ParsedMessage &msg) {
	logger.info("handleSerialEvent: command=" + msg.command + ", key=" + msg.key + ", value=" + msg.value);
	sendResponse(client, "serial", "response", "success", "Serial Event verarbeitet", "");
}

/**
 * @brief Sendet eine Antwort über WebSocket an den Client.
 *
 * @param client Ziel-Client.
 * @param event Ereignistyp als String.
 * @param action Die ausgeführte Aktion.
 * @param status Status der Aktion ("success", "error", ...).
 * @param details Zusätzliche Details zur Aktion.
 * @param error Optionaler Fehlertext.
 */
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const String &details,
                  const String &error) {
	StaticJsonDocument<512> jsonResponse;
	jsonResponse["event"] = event;
	jsonResponse["action"] = action;
	jsonResponse["status"] = status;
	jsonResponse["details"] = details;
	jsonResponse["error"] = error;
	String response;
	serializeJson(jsonResponse, response);
	client->text(response);
}

/**
 * @brief Sendet eine Antwort über WebSocket an den Client.
 *
 * @param client Ziel-Client.
 * @param event Ereignistyp als String.
 * @param action Die ausgeführte Aktion.
 * @param status Status der Aktion ("success", "error", ...).
 * @param details Zusätzliche Details zur Aktion.
 * @param error Optionaler Fehlertext.
 */
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const JsonArray &details,
                  const String &error) {
	StaticJsonDocument<1024> jsonResponse;
	jsonResponse["event"] = event;
	jsonResponse["action"] = action;
	jsonResponse["status"] = status;
	jsonResponse["details"] = details;
	jsonResponse["error"] = error;
	String response;
	serializeJson(jsonResponse, response);
	client->text(response);
}
