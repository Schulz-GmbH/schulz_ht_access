#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// AsyncWebServerResponse *jsonResponse(AsyncWebServerRequest *request, int statusCode, const String &message);
extern AsyncWebServerResponse *jsonResponse(AsyncWebServerRequest *request, int statusCode, const String &message);

#endif  // RESPONSE_HANDLER_H
