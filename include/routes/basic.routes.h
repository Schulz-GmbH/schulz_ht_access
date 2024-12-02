#ifndef BASIC_ROUTES_H
#define BASIC_ROUTES_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <LittleFS.h>

void initBasicRoutes(AsyncWebServer &server);

#endif
