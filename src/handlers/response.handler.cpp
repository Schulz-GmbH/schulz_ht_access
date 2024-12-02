#include "../include/handlers/response.handler.h"

AsyncWebServerResponse *jsonResponse(AsyncWebServerRequest *request, int statusCode, const String &message) {
	String responseBody = "{\"status\": \"" + message + "\"}";
	return request->beginResponse(statusCode, "application/json", responseBody);
}
