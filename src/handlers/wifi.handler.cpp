#include "handlers/wifi.handler.h"

#include <WiFi.h>

#include "handlers/preferences.handler.h"
#include "handlers/response.handler.h"

bool isWiFiEnabled = true;

void handleWiFiEvent(const String &setting, const String &value, AsyncWebSocketClient *client) {
	if (setting == "get") {
		preferences.begin("wifi-config", true);  // Namespace im Lesemodus öffnen
		String storedSSID = preferences.getString("ssid", "");
		String storedPassword = preferences.getString("password", "");
		preferences.end();

		if (storedSSID.isEmpty()) {
			sendResponse(client, "wifi", "get", "off", "no ssid or password", "no ssid");
		} else {
			sendResponse(client, "wifi", "get", "on", storedSSID);
		}
	} else if (setting == "set") {
		int separatorIndex = value.indexOf(':');
		if (separatorIndex != -1) {
			preferences.begin("wifi-config", false);
			String ssid = value.substring(0, separatorIndex);
			String password = value.substring(separatorIndex + 1);

			preferences.putString("ssid", ssid);
			preferences.putString("password", password);
			preferences.end();

			sendResponse(client, "wifi", "set", "success", ssid);
		} else {
			sendResponse(client, "wifi", "set", "error", "", "invalid format");
		}
	} else if (setting == "status") {
		if (WiFi.status() == WL_CONNECTED) {
			StaticJsonDocument<512> doc;
			JsonArray details = doc.createNestedArray();

			// Key-Value-Paare als Objekte zum Array hinzufügen
			JsonObject item1 = details.createNestedObject();
			item1["ssid"] = WiFi.SSID();

			sendResponse(client, "wifi", "status", "connected", details, "");
		} else {
			sendResponse(client, "wifi", "status", "disconnected", "", "not connected to any network");
		}
	} else if (setting == "connect") {
		WiFi.begin(preferences.getString("ssid", "").c_str(), preferences.getString("password", "").c_str());

		unsigned long startAttemptTime = millis();
		while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
			delay(500);
		}

		if (WiFi.status() == WL_CONNECTED) {
			sendResponse(client, "wifi", "connect", "on", "Connected to: " + WiFi.SSID());
		} else {
			sendResponse(client, "wifi", "connect", "off", "", "connection failed");
		}
	} else if (setting == "disconnect") {
		WiFi.disconnect();
		sendResponse(client, "wifi", "disconnect", "off", "WiFi disconnected");
	} else {
		sendResponse(client, "wifi", "error", "unknown", "", "unknown setting");
	}
}

bool connectToWiFi(const String &ssid, const String &password) {
	if (!isWiFiEnabled) {
		return false;
	}

	WiFi.begin(ssid.c_str(), password.c_str());
	unsigned long startAttemptTime = millis();

	while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
		delay(500);
		yield();
		Serial.print(".");
	}

	if (WiFi.status() == WL_CONNECTED) {
		Serial.println("\nSTA-Modus verbunden! IP: " + WiFi.localIP().toString());
	} else {
		// Serial.println("Verbindung im STA-Modus fehlgeschlagen. Fehler: " + getWiFiError(WiFi.status()));
		isWiFiEnabled = false;
		return false;
	}

	isWiFiEnabled = true;
	return true;
}

bool readWiFiConfig(String &ssid, String &password) {
	preferences.begin("wifi-config", true);
	ssid = preferences.getString("ssid", "");
	password = preferences.getString("password", "");
	preferences.end();
	return !ssid.isEmpty() && !password.isEmpty();
}

// #include "handlers/wifi.handler.h"

// Preferences preferences;

// // Globale Variable
// bool isWiFiEnabled = true;

// // WLAN-Daten einlesen
// bool readWiFiConfig(String &ssid, String &password) {
// 	preferences.begin("wifi-config", true);            // Namespace im Lesemodus öffnen
// 	ssid = preferences.getString("ssid", "");          // Standardwert: ""
// 	password = preferences.getString("password", "");  // Standardwert: ""
// 	preferences.end();                                 // Namespace schließen

// 	if (ssid.isEmpty() || password.isEmpty()) {
// 		Serial.println("Keine gespeicherten WLAN-Daten gefunden.");
// 		return false;
// 	} else {
// 		Serial.println("WLAN-Daten erfolgreich eingelesen.");
// 		Serial.println("SSID: " + ssid);
// 		return true;
// 	}

// 	return true;
// }

// // WLAN-Daten speichern
// bool saveWiFiConfig(const String &ssid, const String &password) {
// 	if (ssid.isEmpty() || password.isEmpty()) {
// 		Serial.println("SSID oder Passwort ist leer. Abbruch.");
// 		return false;  // Speichern nicht möglich
// 	}  // Namespace schließen

// 	preferences.begin("wifi-config", false);                           // Namespace öffnen, Schreibmodus
// 	bool ssidSaved = preferences.putString("ssid", ssid);              // SSID speichern
// 	bool passwordSaved = preferences.putString("password", password);  // Passwort speichern
// 	preferences.end();                                                 // Namespace schließen

// 	if (ssidSaved && passwordSaved) {
// 		Serial.println("WLAN-Daten erfolgreich gespeichert.");
// 		return true;
// 	} else {
// 		Serial.println("Fehler beim Speichern der WLAN-Daten.");
// 		return false;
// 	}
// }

// // WLAN verbinden
// String connectToWiFi() {
// 	if (!isWiFiEnabled) {
// 		return "WLAN ist deaktiviert. Verbindung wird nicht hergestellt.";
// 	}

// 	preferences.begin("wifi-config", true);  // Namespace im Lesemodus öffnen
// 	String ssid = preferences.getString("ssid", "");
// 	String password = preferences.getString("password", "");
// 	preferences.end();

// 	if (ssid.isEmpty() || password.isEmpty()) {
// 		return "Keine gespeicherte WiFi-Konfiguration gefunden. Verbindung wird nicht hergestellt.";
// 	}

// 	WiFi.begin(ssid.c_str(), password.c_str());
// 	String response = "Verbinde mit WLAN: " + ssid;
// 	Serial.println(response);

// 	unsigned long startAttemptTime = millis();
// 	while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
// 		delay(100);
// 		yield();  // Task-Scheduler aufrufen
// 		Serial.print(".");
// 	}

// 	if (WiFi.status() == WL_CONNECTED) {
// 		response = "STA-Modus verbunden! IP: " + WiFi.localIP().toString();
// 	} else {
// 		response = "Verbindung im STA-Modus fehlgeschlagen. Fehler: " + getWiFiError(WiFi.status());
// 	}

// 	return response;
// }

// // WLAN-Status prüfen
// String getWiFiStatus() {
// 	if (!isWiFiEnabled) {
// 		return "{\"enabled\": false, \"connected\": false}";
// 	}

// 	bool connected = (WiFi.status() == WL_CONNECTED);
// 	return "{\"enabled\": true, \"connected\": " + String(connected ? "true" : "false") + "}";
// }

// // WLAN-Fehler ausgeben
// String getWiFiError(wl_status_t status) {
// 	switch (status) {
// 		case WL_IDLE_STATUS:
// 			return "Status: WL_IDLE_STATUS - WLAN ist inaktiv.";
// 		case WL_NO_SSID_AVAIL:
// 			return "Status: WL_NO_SSID_AVAIL - SSID nicht gefunden.";
// 		case WL_SCAN_COMPLETED:
// 			return "Status: WL_SCAN_COMPLETED - Scan abgeschlossen, aber keine Verbindung.";
// 		case WL_CONNECT_FAILED:
// 			return "Status: WL_CONNECT_FAILED - Verbindung fehlgeschlagen. Überprüfe das Passwort.";
// 		case WL_CONNECTION_LOST:
// 			return "Status: WL_CONNECTION_LOST - Verbindung verloren.";
// 		case WL_DISCONNECTED:
// 			return "Status: WL_DISCONNECTED - Keine Verbindung zu einem Access Point.";
// 		default:
// 			return "Status: Unbekannter Fehler.";
// 	}
// 	return "Status: Unbekannter Fehler.";
// }

// String urlDecode(const String &input) {
// 	String decoded = "";
// 	char a, b;
// 	for (size_t i = 0; i < input.length(); i++) {
// 		if ((input[i] == '%') && ((a = input[i + 1]) && (b = input[i + 2])) && isxdigit(a) && isxdigit(b)) {
// 			a = a <= '9' ? a - '0' : toupper(a) - 'A' + 10;
// 			b = b <= '9' ? b - '0' : toupper(b) - 'A' + 10;
// 			decoded += char(16 * a + b);
// 			i += 2;
// 		} else if (input[i] == '+') {
// 			decoded += ' ';
// 		} else {
// 			decoded += input[i];
// 		}
// 	}
// 	return decoded;
// }
