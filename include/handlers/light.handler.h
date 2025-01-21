#ifndef LIGHT_HANDLER_H
#define LIGHT_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

extern const int RED_LED;
extern const int GREEN_LED;
extern const int YELLOW_LED;

extern TaskHandle_t redBlinkTaskHandle;
extern TaskHandle_t greenBlinkTaskHandle;
extern TaskHandle_t yellowBlinkTaskHandle;

void handleLightEvent(const String &setting, const String &value, AsyncWebSocketClient *client);

#endif
