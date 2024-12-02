#ifndef WIFI_ROUTES_H
#define WIFI_ROUTES_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include "../include/handlers/response.handler.h"
#include "../include/handlers/wifi.handler.h"

void initWiFiRoutes(AsyncWebServer &server);

#endif
