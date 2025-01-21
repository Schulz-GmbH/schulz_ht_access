#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

// Funktion für detaillierte WebSocket-Antworten
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const String &details = "",
                  const String &error = "");

#endif
