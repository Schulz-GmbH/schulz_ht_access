#include "handlers/system.handler.h"

#include "handlers/helper.handler.h"
#include "handlers/preferences.handler.h"
#include "handlers/response.handler.h"

void handleSystemEvent(const String &setting, const String &value, AsyncWebSocketClient *client) {
	if (setting == "get" && value == "version") {
		// JSON-Dokument für das `details`-Array erstellen
		StaticJsonDocument<512> doc;
		JsonArray details = doc.createNestedArray();

		// Key-Value-Paare als Objekte zum Array hinzufügen
		JsonObject item1 = details.createNestedObject();
		item1["version"] = String(FIRMWARE_VERSION);

		sendResponse(client, "system", "get", "success", details, "");
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
		item1["IP"] = ip;

		JsonObject item2 = details.createNestedObject();
		item2["Port"] = port;

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
