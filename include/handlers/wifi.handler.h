#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>

extern bool isWiFiEnabled;  // WLAN-Status

bool readWiFiConfig(String &ssid, String &password);
bool saveWiFiConfig(const String &ssid, const String &password);
void connectToWiFi(const String &ssid, const String &password);

String getWiFiStatus();
void printWiFiError(wl_status_t status);

String urlDecode(const String &input);

#endif
