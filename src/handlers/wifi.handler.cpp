#include "handlers/wifi.handler.h"

#include <Preferences.h>

Preferences preferences;

// Globale Variable
bool isWiFiEnabled = true;

// WLAN-Daten einlesen
bool readWiFiConfig(String &ssid, String &password) {
	preferences.begin("wifi-config", true);            // Namespace im Lesemodus öffnen
	ssid = preferences.getString("ssid", "");          // Standardwert: ""
	password = preferences.getString("password", "");  // Standardwert: ""
	preferences.end();                                 // Namespace schließen

	if (ssid.isEmpty() || password.isEmpty()) {
		Serial.println("Keine gespeicherten WLAN-Daten gefunden.");
		return false;
	}

	return true;
}

// WLAN-Daten speichern
bool saveWiFiConfig(const String &ssid, const String &password) {
	if (ssid.isEmpty() || password.isEmpty()) {
		Serial.println("SSID oder Passwort ist leer. Abbruch.");
		return false;  // Speichern nicht möglich
	}  // Namespace schließen

	preferences.begin("wifi-config", false);                           // Namespace öffnen, Schreibmodus
	bool ssidSaved = preferences.putString("ssid", ssid);              // SSID speichern
	bool passwordSaved = preferences.putString("password", password);  // Passwort speichern
	preferences.end();                                                 // Namespace schließen

	if (ssidSaved && passwordSaved) {
		Serial.println("WLAN-Daten erfolgreich gespeichert.");
		return true;
	} else {
		Serial.println("Fehler beim Speichern der WLAN-Daten.");
		return false;
	}
}

// WLAN verbinden
void connectToWiFi(const String &ssid, const String &password) {
	if (!isWiFiEnabled) {
		Serial.println("WLAN ist deaktiviert. Verbindung wird nicht hergestellt.");
		return;
	}

	if (ssid.isEmpty() || password.isEmpty()) {
		Serial.println("SSID oder Passwort ist leer. WLAN-Verbindung wird nicht hergestellt.");
		return;
	}

	WiFi.begin(ssid.c_str(), password.c_str());
	Serial.printf("Verbinde mit WLAN: %s\n", ssid.c_str());
	unsigned long startAttemptTime = millis();

	while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
		delay(100);
		yield();  // Task-Scheduler aufrufen
		Serial.print(".");
	}

	if (WiFi.status() == WL_CONNECTED) {
		Serial.println("\nSTA-Modus verbunden!");
		Serial.print("IP-Adresse im Heimnetz: ");
		Serial.println(WiFi.localIP());
	} else {
		Serial.println("\nVerbindung im STA-Modus fehlgeschlagen.");
		printWiFiError(WiFi.status());
	}
}

// WLAN-Status prüfen
String getWiFiStatus() {
	if (!isWiFiEnabled) {
		return "{\"enabled\": false, \"connected\": false}";
	}

	bool connected = (WiFi.status() == WL_CONNECTED);
	return "{\"enabled\": true, \"connected\": " + String(connected ? "true" : "false") + "}";
}

// WLAN-Fehler ausgeben
void printWiFiError(wl_status_t status) {
	switch (status) {
		case WL_IDLE_STATUS:
			Serial.println("Status: WL_IDLE_STATUS - WLAN ist inaktiv.");
			break;
		case WL_NO_SSID_AVAIL:
			Serial.println("Status: WL_NO_SSID_AVAIL - SSID nicht gefunden.");
			break;
		case WL_SCAN_COMPLETED:
			Serial.println("Status: WL_SCAN_COMPLETED - Scan abgeschlossen, aber keine Verbindung.");
			break;
		case WL_CONNECT_FAILED:
			Serial.println("Status: WL_CONNECT_FAILED - Verbindung fehlgeschlagen. Überprüfe das Passwort.");
			break;
		case WL_CONNECTION_LOST:
			Serial.println("Status: WL_CONNECTION_LOST - Verbindung verloren.");
			break;
		case WL_DISCONNECTED:
			Serial.println("Status: WL_DISCONNECTED - Keine Verbindung zu einem Access Point.");
			break;
		default:
			Serial.println("Status: Unbekannter Fehler.");
			break;
	}
}

String urlDecode(const String &input) {
	String decoded = "";
	char a, b;
	for (size_t i = 0; i < input.length(); i++) {
		if ((input[i] == '%') && ((a = input[i + 1]) && (b = input[i + 2])) && isxdigit(a) && isxdigit(b)) {
			a = a <= '9' ? a - '0' : toupper(a) - 'A' + 10;
			b = b <= '9' ? b - '0' : toupper(b) - 'A' + 10;
			decoded += char(16 * a + b);
			i += 2;
		} else if (input[i] == '+') {
			decoded += ' ';
		} else {
			decoded += input[i];
		}
	}
	return decoded;
}
