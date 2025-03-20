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
			logger.info("WebSocket-Client " + String(client->id()) + " verbunden.");
			break;

		case WS_EVT_DISCONNECT:
			logger.info("WebSocket-Client" + String(client->id()) + " getrennt.");
			break;

		case WS_EVT_ERROR:
			logger.info("WebSocket-Fehler bei Client " + String(client->id()) + ".");
			break;

		case WS_EVT_PONG:
			logger.info("WebSocket-Ping/Pong von Client " + String(client->id()) + " .");
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

	// Prüfe den Command-Typ und führe die entsprechenden Aktionen aus
	if (msg.command == "wifi") {
		if (msg.key == "get") {
			// Liefert die SSID und das Passwort
			String ssid, password;
			wifiManager.getSTAConfig(ssid, password);

			StaticJsonDocument<256> doc;
			JsonArray details = doc.createNestedArray("details");
			details.add(ssid);      // SSID als erstes Element
			details.add(password);  // Passwort als zweites Element

			// Antwort senden mit Array
			sendResponse(client, "system", "wifi", "success", details, "");
		} else if (msg.key == "set") {
			// Setzt SSID und Passwort
			DynamicJsonDocument doc(1024);
			deserializeJson(doc, msg.value);
			String ssid = doc["ssid"];
			String password = doc["password"];
			wifiManager.setSTAConfig(ssid, password);
			sendResponse(client, "system", "wifi", "success", "SSID und Passwort gesetzt", "");
		} else if (msg.key == "status") {
			// Überprüfen, ob das STA verbunden ist oder nicht
			String connectionStatus;
			if (WiFi.status() == WL_CONNECTED) {
				connectionStatus = "true";
			} else {
				connectionStatus = "false";
			}

			// Antwort senden
			sendResponse(client, "system", "response", "success", connectionStatus, "");

		} else if (msg.key == "connect") {
			// Verbindet das Wifi mit den gespeicherten Zugangsdaten
			if (wifiManager.connectSTA()) {
				sendResponse(client, "system", "wifi", "success", "Verbindung erfolgreich", "");
			} else {
				sendResponse(client, "system", "wifi", "error", "Verbindung fehlgeschlagen", "");
			}
		} else if (msg.key == "disconnect") {
			// Trennt die Verbindung zum Wifi
			wifiManager.disconnectSTA();
			sendResponse(client, "system", "wifi", "success", "Verbindung getrennt", "");
		} else if (msg.key == "enable") {
			// Aktiviert Wifi
			WiFi.mode(WIFI_AP_STA);
			sendResponse(client, "system", "wifi", "success", "Wifi aktiviert", "");
		} else if (msg.key == "disable") {
			// Deaktiviert Wifi
			WiFi.mode(WIFI_OFF);
			sendResponse(client, "system", "wifi", "success", "Wifi deaktiviert", "");
		} else if (msg.key == "list") {
			// Liefert ein Array mit allen gespeicherten Netzwerken
			std::vector<WiFiNetwork> savedNetworks = wifiManager.getSavedNetworks();

			StaticJsonDocument<1024> doc;
			JsonArray networkList = doc.to<JsonArray>();

			for (const auto &network : savedNetworks) {
				JsonObject networkObj = networkList.createNestedObject();
				networkObj["ssid"] = network.ssid;
				networkObj["password"] = network.password;
			}

			String jsonString;
			serializeJson(doc, jsonString);

			sendResponse(client, "system", "wifi", "success", networkList, "");
		} else if (msg.key == "scan") {
			xTaskCreate(scanNetworksTask, "ScanNetworks", 8192, client, 1, NULL);
		} else if (msg.key == "add") {
			// Fügt ein Netzwerk zu den bekannten Netzwerken hinzu
			DynamicJsonDocument doc(1024);
			deserializeJson(doc, msg.value);
			String ssid = doc["ssid"];
			String password = doc["password"];
			wifiManager.addNetwork(ssid, password);
			sendResponse(client, "system", "wifi", "success", "Netzwerk hinzugefügt", "");
		} else if (msg.key == "remove") {
			// Entfernt ein Netzwerk aus den bekannten Netzwerken
			String ssid = msg.value;
			if (wifiManager.removeNetwork(ssid)) {
				sendResponse(client, "system", "wifi", "success", "Netzwerk entfernt", "");
			} else {
				sendResponse(client, "system", "wifi", "error", "Netzwerk nicht gefunden", "");
			}
		}
	} else if (msg.command == "version" && msg.key == "get") {
		// Gibt die aktuelle Firmware-Version zurück
		sendResponse(client, "system", "version", "success", FIRMWARE_VERSION, "");
	} else {
		sendResponse(client, "system", "response", "error", "Unbekannter Command oder Key", "");
	}
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

/**
 * @brief FreeRTOS-Task zum Scannen verfügbarer WLAN-Netzwerke und zur Übermittlung der Ergebnisse via WebSocket.
 *
 * Diese Task führt einen Scan nach verfügbaren WLAN-Netzwerken durch, erstellt daraus ein JSON-formatiertes
 * Array mit allen gefundenen Netzwerken inklusive SSID, Signalstärke (RSSI), Verschlüsselungstyp und Kanal.
 *
 * Nach Abschluss des Scans werden die Ergebnisse als JSON-basierte WebSocket-Antwort an den anfragenden Client gesendet.
 * Wird kein Netzwerk gefunden, erhält der Client ein leeres Array als Antwort.
 *
 * Die Task löscht sich nach erfolgreichem Versand der Ergebnisse automatisch selbst.
 *
 * @param parameter Pointer auf den AsyncWebSocketClient, der den Scan angefordert hat.
 */
void scanNetworksTask(void *parameter) {
	std::vector<ScannedNetwork> scannedNetworks = wifiManager.scanNetworks();
	StaticJsonDocument<1024> doc;

	// Erzeuge ein Array "networks" im JSON-Dokument
	JsonArray networkList = doc.createNestedArray("networks");

	// Prüfe, ob überhaupt Netzwerke gefunden wurden
	if (scannedNetworks.empty()) {
		logger.info("Keine Netzwerke gefunden.");
		// Leeres Array zum Senden vorbereiten
		JsonArray emptyArray;
		sendResponse((AsyncWebSocketClient *)parameter, "system", "wifi", "success", emptyArray, "");
	} else {
		// Für jedes gefundene Netzwerk ein JSON-Objekt erstellen
		for (const auto &net : scannedNetworks) {
			JsonObject networkObj = networkList.createNestedObject();
			networkObj["ssid"] = net.ssid;
			networkObj["rssi"] = net.rssi;
			networkObj["encryptionType"] = net.encryptionType;
			networkObj["channel"] = net.channel;

			logger.info("SSID=" + net.ssid + " RSSI=" + String(net.rssi) + " Kanal=" + String(net.channel));
		}

		logger.info("Scan abgeschlossen: " + String(scannedNetworks.size()) + " Netzwerke gefunden.");

		// Sende das komplette Array an den Client
		sendResponse((AsyncWebSocketClient *)parameter, "system", "wifi", "success", networkList, "");
	}
	// Lösche den Task, wenn das Scannen abgeschlossen ist
	vTaskDelete(NULL);
}
