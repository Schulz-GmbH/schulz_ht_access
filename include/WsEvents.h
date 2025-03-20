#ifndef WS_EVENTS_H
#define WS_EVENTS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "LLog.h"
#include "WiFiManager.h"

// Enum für die WebSocket-Events
enum WsEvents { WS_EVT_SYSTEM, WS_EVT_LOG, WS_EVT_SERIAL };

// Struktur für die geparsten Nachrichten
struct ParsedMessage {
	WsEvents eventType;
	String command;
	String key;
	String value;
};

// Funktionen für WebSocket-Verarbeitung
ParsedMessage parseWebSocketMessage(const char *jsonData);
void handleSystemEvent(AsyncWebSocketClient *client, const ParsedMessage &msg);
void handleLogEvent(AsyncWebSocketClient *client, const ParsedMessage &msg);
void handleSerialEvent(AsyncWebSocketClient *client, const ParsedMessage &msg);

// Prototypen für sendResponse-Funktionen:
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const String &details,
                  const String &error);
void sendResponse(AsyncWebSocketClient *client, const String &event, const String &action, const String &status, const JsonArray &details,
                  const String &error);

// Prototyp für den WebSocket-Event-Handler:
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

void scanNetworksTask(void *parameter);

#endif
