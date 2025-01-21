#ifndef SYSTEM_HANDLER_H
#define SYSTEM_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void handleSystemEvent(const String &setting, const String &value, AsyncWebSocketClient *client);

#endif
