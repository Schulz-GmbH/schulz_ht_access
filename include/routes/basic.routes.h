#ifndef BASIC_ROUTES_H
#define BASIC_ROUTES_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

void initBasicRoutes(AsyncWebServer &server);
void handleFileRequest(AsyncWebServerRequest *request, const String &subPath);
String getContentType(String path);

#endif
