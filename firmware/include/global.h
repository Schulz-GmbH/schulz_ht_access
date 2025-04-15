#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <IPAddress.h>
#include <Preferences.h>

// Firmware-Version
#define FIRMWARE_VERSION "1.0.0"
// WebInterface-Version
#define WEB_VERSION "1.0.0"

// Serielle Pins (UART2)
#define RXD2 16
#define TXD2 17

// LED-Pins
#define RED_LED 33
#define GREEN_LED 26
#define YELLOW_LED 27

extern Preferences preferences;

// Feste Zugangsdaten für den Access Point
static const char *AP_SSID = "ESP32-AccessPoint";
static const char *AP_PASSWORD = "12345678";

// IP-Konfiguration für den Access Point
static const IPAddress AP_LOCAL_IP(192, 168, 5, 1);
static const IPAddress AP_GATEWAY(192, 168, 5, 1);
static const IPAddress AP_SUBNET(255, 255, 255, 0);

#endif
