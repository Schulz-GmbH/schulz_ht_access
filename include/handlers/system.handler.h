#ifndef SYSTEM_HANDLER_H
#define SYSTEM_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <Preferences.h>

// Firmware-Version im Code hinterlegen
#define FIRMWARE_VERSION "1.0.0"

void handleSystemEvent(const String &setting, const String &value, AsyncWebSocketClient *client);

#endif
