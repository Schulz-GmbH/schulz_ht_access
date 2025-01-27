#include "handlers/response.handler.h"

// Funktion für detaillierte WebSocket-Antworten
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const String &details,
                  const String &error) {
	// JSON-Dokument erstellen
	StaticJsonDocument<512> jsonResponse;
	jsonResponse["event"] = event;      // Haupt-Event (z.B. wifi, light, etc.)
	jsonResponse["action"] = action;    // Ausgeführte Aktion (z.B. get, set, connect, etc.)
	jsonResponse["status"] = status;    // Status (z.B. on, off, success, error, etc.)
	jsonResponse["details"] = details;  // Zusätzliche Informationen (z.B. welche Lampe, SSID, etc.)
	jsonResponse["error"] = error;      // Fehlernachricht, falls vorhanden

	// JSON in einen String konvertieren
	String response;
	serializeJson(jsonResponse, response);

	// Antwort an den Client senden
	client->text(response);
}

void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const JsonArray &details,
                  const String &error) {
	// JSON-Dokument erstellen
	StaticJsonDocument<1024> jsonResponse;
	jsonResponse["event"] = event;      // Haupt-Event (z.B. wifi, light, etc.)
	jsonResponse["action"] = action;    // Ausgeführte Aktion (z.B. get, set, connect, etc.)
	jsonResponse["status"] = status;    // Status (z.B. on, off, success, error, etc.)
	jsonResponse["details"] = details;  // Details als Array von Objekten
	jsonResponse["error"] = error;      // Fehlernachricht, falls vorhanden

	// JSON in einen String konvertieren
	String response;
	serializeJson(jsonResponse, response);

	// Antwort an den Client senden
	client->text(response);
}
