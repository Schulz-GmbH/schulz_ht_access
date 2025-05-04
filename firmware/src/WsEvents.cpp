/**
 * @file WsEvents.cpp
 * @brief Implementierung zur Verarbeitung von WebSocket-Nachrichten nach Event-Typen.
 *
 * Dieses Modul verarbeitet JSON-formatierte WebSocket-Nachrichten und leitet sie an
 * spezifische Event-Handler weiter: `system`, `log` oder `serial`. Es unterstützt
 * zudem den Verbindungsaufbau, Task-basiertes Netzwerk-Scannen und asynchrone
 * Verbindung mit Zugangsdaten.
 */

#include "WsEvents.h"

#include <LittleFS.h>

#include "SerialBridge.h"

extern SerialBridge *serialBridge;

/**
 * @brief Konvertiert einen Event-Typ-String in das passende Enum.
 *
 * @param type Typname aus dem JSON ("system", "log", "serial").
 * @return Enum-Wert des entsprechenden Typs.
 */
WsEvents getEventType(const String &type) {
	if (type == "system") return WS_EVT_SYSTEM;
	if (type == "log") return WS_EVT_LOG;
	if (type == "serial") return WS_EVT_SERIAL;
	return WS_EVT_SYSTEM;
}

/**
 * @brief Parst eine WebSocket-Nachricht aus JSON zu einer `ParsedMessage`.
 *
 * @param jsonData Die rohen JSON-Daten.
 * @return ParsedMessage mit Feldern `eventType`, `command`, `key`, `value`.
 */
ParsedMessage parseWebSocketMessage(const char *jsonData) {
	ParsedMessage msg{WS_EVT_SYSTEM, "", "", ""};
	StaticJsonDocument<256> doc;
	if (deserializeJson(doc, jsonData) == DeserializationError::Ok) {
		msg.eventType = getEventType(doc["type"].as<String>());
		msg.command = doc["command"].as<String>();
		msg.key = doc["key"].as<String>();
		msg.value = doc["value"].as<String>();
	}
	return msg;
}

/**
 * @brief Behandelt WebSocket-Nachrichten vom Typ "system".
 *
 * @param client Der WebSocket-Client.
 * @param msg Die geparste Nachricht.
 */
void handleSystemEvent(AsyncWebSocketClient *client, const ParsedMessage &msg) {
	if (msg.command != "wifi") {
		sendResponse(client, "system", "response", "error", "", "Unknown command");
		return;
	}
	if (msg.key == "get") {
		String ssid = wifiManager.currentNetwork();
		StaticJsonDocument<128> doc;
		auto arr = doc.createNestedArray("details");
		arr.add(ssid);
		sendResponse(client, "system", "wifi", "network", arr, "");
	} else if (msg.key == "set") {
		DynamicJsonDocument req(256);
		if (deserializeJson(req, msg.value) != DeserializationError::Ok) {
			sendResponse(client, "system", "wifi", "connect", "false", "Invalid JSON");
			return;
		}
		String ssid = req["ssid"].as<String>();
		String password = req["password"].as<String>();
		// spawn connectTask
		struct Params {
			AsyncWebSocketClient *c;
			String ssid, pwd;
		};
		auto p = new Params{client, ssid, password};
		xTaskCreatePinnedToCore(connectNetworkTask, "connNet", 16384, p, 1, nullptr, 1);
	} else if (msg.key == "status") {
		bool ok = WiFi.status() == WL_CONNECTED;
		sendResponse(client, "system", "wifi", "status", ok ? "connected" : "disconnected", "");
	} else if (msg.key == "connect") {
		bool ok = wifiManager.connectSaved();
		sendResponse(client, "system", "wifi", "connected", ok ? "true" : "false", ok ? "" : "No saved network in range");
	} else if (msg.key == "disconnect") {
		wifiManager.disconnect();
		sendResponse(client, "system", "wifi", "disconnect", "true", "");
	} else if (msg.key == "enable") {
		wifiManager.activate();
		sendResponse(client, "system", "wifi", "activate", "true", "");
	} else if (msg.key == "disable") {
		wifiManager.deactivate();
		sendResponse(client, "system", "wifi", "deactivate", "true", "");
	} else if (msg.key == "list") {
		auto nets = wifiManager.listNetworks();
		StaticJsonDocument<384> doc;
		auto arr = doc.to<JsonArray>();
		for (auto &n : nets) {
			JsonObject o = arr.createNestedObject();
			o["ssid"] = n.ssid;
			o["password"] = n.password;
		}
		sendResponse(client, "system", "wifi", "list", arr, "");
	} else if (msg.key == "scan") {
		xTaskCreatePinnedToCore(scanNetworksTask, "scanNet", 8192, client, 1, nullptr, 1);
	} else {
		sendResponse(client, "system", "wifi", "error", "", "Unknown key");
	}
}

/**
 * @brief Behandelt WebSocket-Nachrichten vom Typ "log".
 *
 * @param client Der WebSocket-Client.
 * @param msg Die geparste Nachricht.
 */
void handleLogEvent(AsyncWebSocketClient *client, const ParsedMessage &msg) {
	// Command-Auswertung:
	if (msg.command == "debug") {
		// 1) log debug <-> activate / deactivate / status
		if (msg.key == "activate") {
			LLog::setFileLogging(true);
			sendDebugResponse(client);
		} else if (msg.key == "deactivate") {
			LLog::setFileLogging(false);
			sendDebugResponse(client);
		} else if (msg.key == "status") {
			bool status = LLog::isFileLogging();
			sendDebugResponse(client);
		} else {
			sendResponse(client, "log", "debug", "error", "Unbekannter Key für 'debug'", "");
		}
	} else {
		// Unbekanntes Command
		sendResponse(client, "log", "response", "error", "Unbekannter Command bei 'log'", "");
	}
}

/**
 * @brief Behandelt WebSocket-Nachrichten vom Typ "serial".
 *
 * @param client Der WebSocket-Client.
 * @param msg Die geparste Nachricht.
 */
void handleSerialEvent(AsyncWebSocketClient *client, const ParsedMessage &msg) {
	if (msg.command == "incoming") {
		sendResponse(client, "serial", "incoming", "success", msg.value);
		return;
	} else if (msg.command == "setBaud") {
		const String &val = msg.value;
		uint32_t newBaud = val.toInt();
		if (!serialBridge->setBaud(newBaud)) {
			sendResponse(client, "serial", "setBaud", "error", "", "Ungültige Baud-Rate");
			return;
		}
	} else if (msg.command == "send") {
		if (serialBridge->isDeviceConnected()) {
			String out = msg.value;
			out.replace("\n", "\r\n");

			serialBridge->sendData(out);
			logger.log({"socket", "info", "device"}, "Gesendet: " + out);
			sendResponse(client, "serial", "send", "success", out, "");
		} else {
			sendResponse(client, "serial", "send", "error", "Serial2 nicht verbunden", "");
		}
		return;
	} else {
		sendResponse(client, "serial", "response", "error", "", "Not implemented");
	}
}

/**
 * @brief FreeRTOS-Task zum Scannen nach WLAN-Netzwerken.
 *
 * Erstellt ein Array aller sichtbaren Netzwerke (SSID + RSSI) und sendet es über WebSocket.
 *
 * @param param AsyncWebSocketClient*, an den das Ergebnis gesendet wird.
 */
void scanNetworksTask(void *param) {
	AsyncWebSocketClient *c = (AsyncWebSocketClient *)param;
	auto nets = wifiManager.scan();
	StaticJsonDocument<512> doc;
	auto arr = doc.createNestedArray("networks");
	for (auto &n : nets) {
		JsonObject o = arr.createNestedObject();
		o["ssid"] = n.ssid;
		o["rssi"] = n.rssi;
	}
	sendResponse(c, "system", "wifi", "scan", arr, "");
	vTaskDelete(nullptr);
}

/**
 * @brief FreeRTOS-Task zum asynchronen Verbindungsaufbau mit einem WLAN.
 *
 * Der Status wird nach Abschluss an den WebSocket-Client gesendet.
 *
 * @param param Zeiger auf `Params`-Struktur (Client, SSID, Passwort).
 */
void connectNetworkTask(void *param) {
	struct Params {
		AsyncWebSocketClient *c;
		String ssid, pwd;
	};
	auto p = static_cast<Params *>(param);
	bool ok = wifiManager.connect(p->ssid, p->pwd);
	sendResponse(p->c, "system", "wifi", ok ? "success" : "error", ok ? "Network saved & connected" : "", ok ? "" : "Connect failed");
	delete p;
	vTaskDelete(nullptr);
}

/**
 * @brief Hilfsfunktion zum Versenden des aktuellen File-Logging-Status.
 *
 * @param client Der WebSocket-Client.
 */
void sendDebugResponse(AsyncWebSocketClient *client) {
	// baue den JSON-Detail-Block
	StaticJsonDocument<128> doc;
	JsonObject det = doc.to<JsonObject>();
	bool status = LLog::isFileLogging();
	det["activate"] = status;
	det["detail"] = status ? "File-Logging aktiviert" : "File-Logging deaktiviert";

	// versende die Antwort
	sendResponse(client, "log", "debug", status ? "success" : "error", det);
}

/**
 * @brief Sendet eine generische WebSocket-Antwort mit `details` als String.
 *
 * @param client Ziel-Client.
 * @param event Ereignis-Typ.
 * @param action Aktion.
 * @param status Status (z.B. "success", "error").
 * @param details Inhaltliche Details.
 * @param error Optionaler Fehlertext.
 */
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const String &details,
                  const String &error) {
	StaticJsonDocument<256> d;
	d["event"] = event;
	d["action"] = action;
	d["status"] = status;
	d["details"] = details;
	d["error"] = error;
	String s;
	serializeJson(d, s);
	client->text(s);
}

/**
 * @deprecated
 * @brief Veraltete Funktion zum Versenden einer WebSocket-Antwort mit einem Array als Details.
 *
 * @param client Ziel-Client.
 * @param event Ereignis-Typ.
 * @param action Aktion.
 * @param status Status.
 * @param details Array mit Details.
 * @param error Fehlertext.
 */
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const JsonArray &details,
                  const String &error) {
	StaticJsonDocument<512> d;
	d["event"] = event;
	d["action"] = action;
	d["status"] = status;
	d["details"] = details;
	d["error"] = error;
	String s;
	serializeJson(d, s);
	client->text(s);
}

/**
 * @brief Sendet eine WebSocket-Antwort mit beliebigem JSON-Wert als Details.
 *
 * @param client Ziel-Client.
 * @param event Ereignis.
 * @param action Aktion.
 * @param status "success" oder "error".
 * @param details JSON-Wert (z. B. Objekt oder Array).
 */
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const JsonVariantConst &details) {
	// Nur erlaubte Status-Werte
	if (status != "success" && status != "error") return;

	StaticJsonDocument<512> d;
	d["event"] = event;
	d["action"] = action;
	d["status"] = status;
	d["details"] = details;

	String s;
	serializeJson(d, s);
	client->text(s);
}
