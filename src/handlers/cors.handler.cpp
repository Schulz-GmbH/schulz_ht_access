#include "handlers/cors.handler.h"

// Middleware-Funktion für CORS
void addCORSHeaders(AsyncWebServerResponse *response) {
	response->addHeader("Access-Control-Allow-Origin", "*");
	response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
	response->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

// Globale Anfragebearbeitung für CORS
void setupCORS(AsyncWebServer &server) {
	server.onNotFound([](AsyncWebServerRequest *request) {
		if (request->method() == HTTP_OPTIONS) {
			AsyncWebServerResponse *response = request->beginResponse(204);
			addCORSHeaders(response);
			request->send(response);
		} else {
			AsyncWebServerResponse *response = request->beginResponse(404, "text/plain", "Not Found");
			addCORSHeaders(response);
			request->send(response);
		}
	});
}
