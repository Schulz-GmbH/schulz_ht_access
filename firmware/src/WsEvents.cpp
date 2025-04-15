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

#include "LittleFS.h"

/**
 * @brief Ermittelt den Typ eines WebSocket-Events anhand eines Strings.
 *
 * @param type Zeichenkette wie "system", "log" oder "serial".
 * @return Der zugehörige WsEvents-Wert.
 */
WsEvents getEventType(const String &type) {
	if (type == "system") return WS_EVT_SYSTEM;
	if (type == "log") return WS_EVT_LOG;
	if (type == "serial") return WS_EVT_SERIAL;
	return WS_EVT_SYSTEM;  // Fallback auf SYSTEM
}

/**
 * @brief Parst eine JSON-WebSocket-Nachricht in eine ParsedMessage-Struktur.
 *
 * @param jsonData Eingehende JSON-Daten.
 * @return ParsedMessage-Struktur mit Event-Typ, Kommando, Key und Wert.
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
 * @brief Haupt-Event-Dispatcher für WebSocket-Ereignisse.
 *
 * @param server WebSocket-Server.
 * @param client Aktueller Client.
 * @param type Typ des WebSocket-Ereignisses.
 * @param arg Zusätzliche Daten (nicht verwendet).
 * @param data Nachrichtendaten.
 * @param len Länge der Nachrichtendaten.
 */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	switch (type) {
		case WS_EVT_CONNECT:
			logger.info("[WebSocket] Client " + String(client->id()) + " verbunden.");
			break;

		case WS_EVT_DISCONNECT:
			logger.info("[WebSocket] Client" + String(client->id()) + " getrennt.");
			break;

		case WS_EVT_ERROR:
			logger.info("[WebSocket] Fehler bei Client " + String(client->id()) + ".");
			break;

		case WS_EVT_PONG:
			logger.info("[WebSocket] Ping/Pong von Client " + String(client->id()) + " .");
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
					logger.warn("[WebSocket] Unbekanntes Event im DATA-Payload!");
			}
			break;
		}
		default:
			logger.warn("[WebSocket] Unbekannter Event!");
			break;
	}
}

/**
 * @brief Verarbeitet WebSocket-Nachrichten vom Typ "system".
 *
 * Diese Funktion wertet das `command`- und `key`-Feld der empfangenen Nachricht aus
 * und führt entsprechende Systemaktionen durch, etwa zur Verwaltung der WLAN-Verbindung.
 *
 * Unterstützte Kommandos:
 * - `wifi` mit `key`: get, set, status, connect, disconnect, enable, disable, list, scan, add, remove
 * - `version` mit `key`: get
 *
 * @param client Client, der das Ereignis gesendet hat.
 * @param msg Die empfangene Nachricht als ParsedMessage.
 */
void handleSystemEvent(AsyncWebSocketClient *client, const ParsedMessage &msg) {
	// Prüfe den Command-Typ und führe die entsprechenden Aktionen aus
	if (msg.command == "wifi") {
		if (msg.key == "get") {
			String ssid, password;
			wifiManager.getNetwork(ssid);

			StaticJsonDocument<256> doc;
			JsonArray details = doc.createNestedArray("details");
			details.add(ssid);

			sendResponse(client, "system", "wifi", "success", details, "");
		} else if (msg.key == "set") {
			DynamicJsonDocument doc(1024);
			deserializeJson(doc, msg.value);
			String ssid = doc["ssid"];
			String password = doc["password"];
			// wifiManager.saveConfig();
			sendResponse(client, "system", "wifi", "success", "SSID und Passwort gesetzt", "");
		} else if (msg.key == "status") {
			uint8_t mode = WiFi.getMode();
			String connectionStatus;

			if (mode == WIFI_OFF || mode == WIFI_AP) {
				connectionStatus = "disabled";
			} else {
				connectionStatus = WiFi.status() == WL_CONNECTED ? "connected" : "disconnected";
			}
			sendResponse(client, "system", "wifi", "status", connectionStatus, "");
		} else if (msg.key == "connect") {
			if (wifiManager.connect()) {
				sendResponse(client, "system", "wifi", "success", "Verbindung erfolgreich", "");
			} else {
				sendResponse(client, "system", "wifi", "error", "Verbindung fehlgeschlagen", "");
			}
		} else if (msg.key == "disconnect") {
			wifiManager.disconnect();
			sendResponse(client, "system", "wifi", "success", "Verbindung getrennt", "");
		} else if (msg.key == "enable") {
			WiFi.mode(WIFI_AP_STA);
			sendResponse(client, "system", "wifi", "success", "Wifi aktiviert", "");
		} else if (msg.key == "disable") {
			WiFi.mode(WIFI_OFF);
			sendResponse(client, "system", "wifi", "success", "Wifi deaktiviert", "");
		} else if (msg.key == "list") {
			std::vector<WiFiNetwork> savedNetworks = wifiManager.listNetwork();

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
			DynamicJsonDocument doc(1024);
			deserializeJson(doc, msg.value);
			String ssid = doc["ssid"];
			String password = doc["password"];
			wifiManager.addNetwork(ssid, password);
			sendResponse(client, "system", "wifi", "success", "Netzwerk hinzugefügt", "");
		} else if (msg.key == "remove") {
			String ssid = msg.value;
			if (wifiManager.removeNetwork(ssid)) {
				sendResponse(client, "system", "wifi", "success", "Netzwerk entfernt", "");
			} else {
				sendResponse(client, "system", "wifi", "error", "Netzwerk nicht gefunden", "");
			}
		}
	} else if (msg.command == "version" && msg.key == "get") {
		sendResponse(client, "system", "version", "success", FIRMWARE_VERSION, "");
	} else {
		sendResponse(client, "system", "response", "error", "Unbekannter Command oder Key", "");
	}
}

/**
 * @brief Verarbeitet WebSocket-Nachrichten vom Typ "log".
 *
 * Diese Funktion verarbeitet WebSocket-Nachrichten mit dem Ereignistyp "log".
 * Sie unterstützt folgende Kommandos:
 *
 * - `debug`:
 *   - `activate`: Aktiviert das Debug-Logging.
 *   - `deactivate`: Deaktiviert das Debug-Logging.
 *   - `status`: Gibt zurück, ob das Debug-Logging aktiv ist.
 *
 * - `files`:
 *   - `list`: Listet alle Logdateien im `/logs`-Verzeichnis.
 *   - `rename`: Bennennt eine vorhandene Logdatei um.
 *   - `delete`: Löscht eine einzelne oder mehrere Logdateien.
 *
 * @param client Der WebSocket-Client, von dem das Ereignis empfangen wurde.
 * @param msg Die parste WebSocket-Nachricht.
 */
void handleLogEvent(AsyncWebSocketClient *client, const ParsedMessage &msg) {
	// Command-Auswertung:
	if (msg.command == "debug") {
		// 1) log debug <-> activate / deactivate / status
		if (msg.key == "activate") {
			LLog::setActive(true);
			sendResponse(client, "log", "debug", "success", "Debug logging aktiviert", "");
		} else if (msg.key == "deactivate") {
			LLog::setActive(false);
			sendResponse(client, "log", "debug", "success", "Debug logging deaktiviert", "");
		} else if (msg.key == "status") {
			bool active = LLog::isActive();
			sendResponse(client, "log", "debug", "success", active ? "true" : "false", "");
		} else {
			sendResponse(client, "log", "debug", "error", "Unbekannter Key für 'debug'", "");
		}
	} else if (msg.command == "files") {
		// 2) log files <-> list / rename / delete
		if (msg.key == "list") {
			// Liste aller .log-Dateien im Ordner /logs
			listLogFiles(client);
		} else if (msg.key == "rename") {
			// Wert parsen => { "fileName": "...", "newFileName": "..." }
			renameLogFile(client, msg.value);
		} else if (msg.key == "delete") {
			// Wert parsen => entweder "fileName": String oder "fileNames": Array
			deleteLogFiles(client, msg.value);
		} else {
			sendResponse(client, "log", "files", "error", "Unbekannter Key für 'files'", "");
		}
	} else {
		// Unbekanntes Command
		sendResponse(client, "log", "response", "error", "Unbekannter Command bei 'log'", "");
	}
}

/**
 * @brief Verarbeitet WebSocket-Nachrichten vom Typ "serial".
 *
 * Diese Funktion dient aktuell als Platzhalter für zukünftige Funktionen zur Steuerung oder
 * Kommunikation über die serielle Schnittstelle. Sie verarbeitet WebSocket-Nachrichten mit dem
 * Ereignistyp `WS_EVT_SERIAL`, analysiert die enthaltenen Kommandos und Schlüssel,
 * und führt entsprechende Aktionen aus.
 *
 * Derzeit wird lediglich der Empfang protokolliert und eine generische Erfolgsmeldung
 * an den Client zurückgeschickt.
 *
 * Unterstützte Felder in der Nachricht:
 * - `command`: Der auszuführende Befehl (aktuell ohne weitere Verarbeitung).
 * - `key`: Weitere Spezifikation des Befehls.
 * - `value`: Optionaler Wert zur Ausführung oder Konfiguration.
 *
 * @param client Zeiger auf den WebSocket-Client, der das Ereignis gesendet hat.
 * @param msg Die parste Nachricht im Format einer `ParsedMessage`-Struktur.
 */
void handleSerialEvent(AsyncWebSocketClient *client, const ParsedMessage &msg) {
	logger.info("handleSerialEvent: command=" + msg.command + ", key=" + msg.key + ", value=" + msg.value);
	sendResponse(client, "serial", "response", "success", "Serial Event verarbeitet", "");
}

/**
 * @brief Sendet eine JSON-Antwort an einen WebSocket-Client.
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

	if (details.length() > 0) {
		logger.info(response);
	} else if (error.length() > 0) {
		logger.error(response);
	} else {
		logger.info("Keine Details oder Fehlermeldung.");
	}
	client->text(response);
}

/**
 * @brief Überladene Version von sendResponse mit JSON-Array.
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

	if (details.size() == 0) {
		logger.error(response);
	} else {
		logger.info(response);
	}

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
	std::vector<Network> scannedNetworks = wifiManager.scan();
	StaticJsonDocument<1024> doc;

	// Erzeuge ein Array "networks" im JSON-Dokument
	JsonArray networkList = doc.createNestedArray("networks");

	// Prüfe, ob überhaupt Netzwerke gefunden wurden
	if (scannedNetworks.empty()) {
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
		}
		// Sende das komplette Array an den Client
		sendResponse((AsyncWebSocketClient *)parameter, "system", "wifi", "success", networkList, "");
	}
	// Lösche den Task, wenn das Scannen abgeschlossen ist
	vTaskDelete(NULL);
}

// Todo: Code nach LLog auslagern?

/**
 * @brief Startet einen FreeRTOS-Task zur Auflistung aller Log-Dateien.
 *
 * Diese Funktion dient als Einstiegspunkt, um asynchron (nicht blockierend)
 * alle vorhandenen Logdateien im `/logs`-Verzeichnis zu erfassen.
 * Die tatsächliche Verarbeitung erfolgt in `listLogFilesTask()`.
 *
 * @param client Zeiger auf den WebSocket-Client, der die Anfrage gesendet hat.
 */
void listLogFiles(AsyncWebSocketClient *client) {
	Serial.println("listLogFiles");
	// Task mit einem großzügigen Stack (hier 8192 Bytes) erstellen
	xTaskCreate(listLogFilesTask, "listLogFilesTask", 8192, client, 1, NULL);
}

/**
 * @brief FreeRTOS-Task zum Durchsuchen des Log-Verzeichnisses und Rückgabe als JSON.
 *
 * Durchsucht das Verzeichnis `/logs` im LittleFS-Dateisystem nach `.log`-Dateien,
 * sammelt deren Namen und Größe und sendet diese Information als JSON-Array
 * zurück an den WebSocket-Client.
 *
 * @param pvParameters Zeiger auf den WebSocket-Client.
 */
void listLogFilesTask(void *pvParameters) {
	AsyncWebSocketClient *client = (AsyncWebSocketClient *)pvParameters;

	// Erhöhe den Speicherpuffer, um Speicherüberläufe zu vermeiden
	StaticJsonDocument<4096> doc;
	JsonArray filesArray = doc.createNestedArray("files");

	// Versuche, das Verzeichnis "/logs" über LittleFS im Lesemodus zu öffnen
	File dir = LittleFS.open("/logs", "r");
	if (!dir) {
		sendResponse(client, "log", "files", "error", "Verzeichnis /logs nicht gefunden oder nicht lesbar", "");
		vTaskDelete(NULL);  // Task löschen
		return;
	}

	// Sicherstellen, dass das geöffnete Objekt ein Verzeichnis ist
	if (!dir.isDirectory()) {
		sendResponse(client, "log", "files", "error", "/logs ist kein Verzeichnis", "");
		dir.close();
		vTaskDelete(NULL);
		return;
	}

	// Durch alle Dateien im Verzeichnis iterieren
	File entry;
	while ((entry = dir.openNextFile())) {
		if (!entry.isDirectory()) {
			String fileName = entry.name();
			// Nur Dateien mit der Endung ".log" berücksichtigen
			if (fileName.endsWith(".log")) {
				JsonObject fileObj = filesArray.createNestedObject();
				fileObj["name"] = fileName;
				fileObj["size"] = (unsigned long)entry.size();
			}
		}
		entry.close();
	}
	dir.close();

	// Sende das JSON-Dokument mit der Liste der Log-Dateien
	sendResponse(client, "log", "files", "success", filesArray, "");

	// Task beenden
	vTaskDelete(NULL);
}

/**
 * @brief Benennt eine bestehende Log-Datei im `/logs`-Verzeichnis um.
 * @deprecated
 *
 * Erwartet eine JSON-Struktur mit `fileName` (alt) und `newFileName` (neu),
 * prüft die Existenz der alten Datei und führt die Umbenennung durch.
 *
 * @param client WebSocket-Client, der die Anfrage gesendet hat.
 * @param jsonValue JSON-String mit den Feldern `fileName` und `newFileName`.
 */
void renameLogFile(AsyncWebSocketClient *client, const String &jsonValue) {
	// JSON parsen
	DynamicJsonDocument doc(256);
	DeserializationError err = deserializeJson(doc, jsonValue);
	if (err) {
		sendResponse(client, "log", "files", "error", "JSON-Parsing fehlgeschlagen", err.c_str());
		return;
	}

	String oldName = doc["fileName"] | "";
	String newName = doc["newFileName"] | "";

	// Safety-Checks
	if (oldName.length() == 0 || newName.length() == 0) {
		sendResponse(client, "log", "files", "error", "Alte oder neue Datei wurde nicht angegeben", "");
		return;
	}

	// Beispiel: /logs/ + name
	String oldPath = "/logs/" + oldName;
	String newPath = "/logs/" + newName;

	// Vorhanden?
	if (!SD.exists(oldPath)) {
		sendResponse(client, "log", "files", "error", "Alte Datei existiert nicht: " + oldName, "");
		return;
	}

	// Umbenennen
	bool success = SD.rename(oldPath, newPath);
	if (!success) {
		sendResponse(client, "log", "files", "error", "Konnte Datei nicht umbenennen", "");
		return;
	}

	sendResponse(client, "log", "files", "success", "Datei umbenannt von " + oldName + " zu " + newName, "");
}

/**
 * @brief Löscht eine oder mehrere Log-Dateien basierend auf einem JSON-Request.
 * @deprecated
 *
 * Das JSON-Objekt kann entweder einen String `fileName` oder ein Array `fileNames` enthalten.
 * Jede Datei wird überprüft und bei Vorhandensein gelöscht.
 *
 * @param client WebSocket-Client, der die Anfrage gestellt hat.
 * @param jsonValue JSON-String mit dem Dateinamen oder einem Array von Dateinamen.
 */
void deleteLogFiles(AsyncWebSocketClient *client, const String &jsonValue) {
	DynamicJsonDocument doc(512);
	DeserializationError err = deserializeJson(doc, jsonValue);
	if (err) {
		sendResponse(client, "log", "files", "error", "JSON-Parsing fehlgeschlagen", err.c_str());
		return;
	}

	// Prüfe zuerst auf "fileName"
	if (doc.containsKey("fileName") && doc["fileName"].is<String>()) {
		String fileName = doc["fileName"].as<String>();
		deleteSingleLogFile(client, fileName);
	}
	// Prüfe auf Array "fileNames"
	else if (doc.containsKey("fileNames") && doc["fileNames"].is<JsonArray>()) {
		JsonArray arr = doc["fileNames"].as<JsonArray>();
		for (JsonVariant v : arr) {
			if (v.is<String>()) {
				deleteSingleLogFile(client, v.as<String>());
			}
		}
		// Am Ende ggf. Erfolgsmeldung
		sendResponse(client, "log", "files", "success", "Mehrere Dateien gelöscht", "");
	} else {
		// Nichts passendes gefunden
		sendResponse(client, "log", "files", "error", "Weder fileName noch fileNames im JSON gefunden.", "");
	}
}

/**
 * @brief Löscht eine einzelne Log-Datei aus dem `/logs`-Verzeichnis.
 * @deprecated
 *
 * Überprüft, ob die Datei existiert, und löscht sie gegebenenfalls. Gibt eine
 * entsprechende WebSocket-Antwort zurück.
 *
 * @param client WebSocket-Client, der die Löschung angefordert hat.
 * @param fileName Name der zu löschenden Datei (nur Dateiname, ohne Pfad).
 */
void deleteSingleLogFile(AsyncWebSocketClient *client, const String &fileName) {
	if (fileName.isEmpty()) return;

	String path = "/logs/" + fileName;
	if (!SD.exists(path)) {
		sendResponse(client, "log", "files", "error", "", "Datei: " + path + " nicht vorhanden");
		return;
	}

	bool removed = SD.remove(path);
	if (removed) {
		sendResponse(client, "log", "files", "success", "Datei: " + path + " gelöscht.", "");
	} else {
		sendResponse(client, "log", "files", "error", "", "Datei: " + path + " konnte nicht gelöscht werden");
	}
}