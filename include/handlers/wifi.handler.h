#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

extern bool isWiFiEnabled;

void handleWiFiEvent(const String &setting, const String &value, AsyncWebSocketClient *client);
bool connectToWiFi(const String &ssid, const String &password);
bool readWiFiConfig(String &ssid, String &password);

#endif
