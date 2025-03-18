#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// Firmware-Version
#define FIRMWARE_VERSION "1.0.0"
// WebInterface-Version
#define WEB_VERSION "1.0.0"

// Serielle Pins (UART2)
#define RXD2 16
#define TXD2 17

// SD-Karten-Pin
#define SD_CS_PIN 5

// LED-Pins
#define RED_LED 33
#define GREEN_LED 32
#define YELLOW_LED 25

extern Preferences preferences;

#endif
