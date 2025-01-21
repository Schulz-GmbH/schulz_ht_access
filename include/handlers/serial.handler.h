#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// Serielle Pins (UART2)
#define RXD2 16
#define TXD2 17

extern bool consoleRunning;

void handleSerialEvent(const String &setting, const String &value, AsyncWebSocketClient *client);

#endif
