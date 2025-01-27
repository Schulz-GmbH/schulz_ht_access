#include "handlers/system.handler.h"

#include "handlers/helper.handler.h"
#include "handlers/preferences.handler.h"
#include "handlers/response.handler.h"

void handleSystemEvent(const String &setting, const String &value, AsyncWebSocketClient *client) {
	if (setting == "get" && value == "version") {
		preferences.begin("api-config", true);  // Namespace im Lesemodus öffnen
		String ip = preferences.getString("api_ip", "");
		String port = preferences.getString("api_port", "");
		preferences.end();

		if (ip.isEmpty() || port.isEmpty()) {
			sendResponse(client, "system", "update", "error", "", "API server not configured");
			return;
		}

		String apiUrl = "http://" + ip + ":" + port + "/api/version";

		HTTPClient http;
		http.begin(apiUrl);

		// API-Server kontaktieren, um auf Updates zu prüfen
		int httpCode = http.GET();
		if (httpCode == 200) {
			// Antwort erfolgreich empfangen
			String payload = http.getString();

			// JSON-Antwort des API-Servers parsen
			StaticJsonDocument<512> doc;
			StaticJsonDocument<512> jsonDoc;
			DeserializationError error = deserializeJson(jsonDoc, payload);
			JsonArray details = doc.createNestedArray();

			if (!error) {
				JsonObject item1 = details.createNestedObject();
				item1["system-version"] = String(FIRMWARE_VERSION);
				item1["available"] = jsonDoc["updateAvailable"];
				item1["server-version"] = jsonDoc["version"];
				item1["url"] = jsonDoc["zipUrl"];

				sendResponse(client, "system", "get", "success", details, "");
			} else {
				sendResponse(client, "system", "update", "error", "", "Failed to parse API response");
			}
		} else {
			sendResponse(client, "system", "update", "error", "", "Failed to contact API server, HTTP code: " + String(httpCode));
		}

		http.end();

	} else if (setting == "get" && value == "api") {
		// API-IP-Adresse und Port abrufen
		if (!preferences.begin("api-config", true)) {  // Namespace im Lesemodus öffnen
			sendResponse(client, "system", "get", "error", "", "Namespace 'api-config' not found");
			return;
		}

		String ip = preferences.getString("api_ip", "");
		String port = preferences.getString("api_port", "");
		preferences.end();

		StaticJsonDocument<512> doc;
		JsonArray details = doc.createNestedArray();

		// Key-Value-Paare als Objekte zum Array hinzufügen
		JsonObject item1 = details.createNestedObject();
		item1["ip"] = ip;

		JsonObject item2 = details.createNestedObject();
		item2["port"] = port;

		sendResponse(client, "system", "get", "success", details, "");
	} else if (setting == "set") {
		char delimiter = ':';
		String values[10];
		int count = 0;
		splitString(value, delimiter, values, count);

		if (count >= 3 && values[0] == "api") {
			preferences.begin("api-config", false);  // Namespace öffnen
			preferences.putString("api_ip", values[1]);
			preferences.putString("api_port", values[2]);
			preferences.end();

			StaticJsonDocument<512> doc;
			JsonArray details = doc.createNestedArray();

			// Key-Value-Paare als Objekte zum Array hinzufügen
			JsonObject item1 = details.createNestedObject();
			item1["IP"] = values[1];

			JsonObject item2 = details.createNestedObject();
			item2["Port"] = values[2];

			sendResponse(client, "system", "set", "success", details, "");
		} else {
			sendResponse(client, "system", "set", "unknown", "", "unknown system setting");
		}
	} else if (setting == "update") {
		// Angenommen, ein Update-Prozess wird hier gestartet
		sendResponse(client, "system", "update", "success", "Update gestartet mit URL: " + value);
	} else {
		sendResponse(client, "system", "set", "error", "", "Invalid API format, expected IP:Port");
	}
}
