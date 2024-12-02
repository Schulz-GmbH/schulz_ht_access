#include "../include/routes/error.routes.h"

#include "../include/handlers/response.handler.h"

// Fehlercode-Tabelle
struct SdErrorCode {
	int code;
	const char *description;
};

SdErrorCode sdErrorCodes[] = {{1, "SD-Karte nicht gefunden"},           {2, "Falsches Dateisystem"},
                              {4, "Schreibgeschützte Karte"},           {13, "Karteninitialisierung fehlgeschlagen"},
                              {28, "Verbindung zur SD-Karte verloren"}, {255, "Unbekannter Fehler"}};

const size_t numErrorCodes = sizeof(sdErrorCodes) / sizeof(sdErrorCodes[0]);

// Route, um die SD-Fehlercodes auszugeben
void initErrorRoutes(AsyncWebServer &server) {
	server.on("/sd/errorcodes", HTTP_GET, [](AsyncWebServerRequest *request) {
		String jsonResponse = "{";
		for (size_t i = 0; i < numErrorCodes; ++i) {
			jsonResponse += "\"Fehlercode " + String(sdErrorCodes[i].code) + "\": \"" + String(sdErrorCodes[i].description) + "\"";
			if (i < numErrorCodes - 1) {
				jsonResponse += ", ";
			}
		}
		jsonResponse += "}";
		request->send(200, "application/json", jsonResponse);
	});
}
