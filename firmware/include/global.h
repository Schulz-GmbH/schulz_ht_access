/**
 * @file global.h
 * @brief Zentrale Definitionen und globale Konstanten für das Projekt.
 *
 * Diese Datei enthält globale Definitionen wie Firmware-Versionen, Hardware-Pinbelegungen,
 * Access-Point-Konfiguration, zulässige Baudraten und globale Variablen.
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <IPAddress.h>
#include <Preferences.h>

/// Aktuelle Firmware-Version
#define FIRMWARE_VERSION "1.0.0"

/// Version des Web-Frontends
#define WEB_VERSION "1.0.0"

// === Serielle Kommunikation (UART2) ===

/// RX-Pin für UART2 (Empfang)
#define RXD2 17

/// TX-Pin für UART2 (Senden)
#define TXD2 16

// === LED-Pinbelegung ===

/// GPIO-Pin für rote LED
#define RED_LED 33

/// GPIO-Pin für grüne LED
#define GREEN_LED 26

/// GPIO-Pin für gelbe LED
#define YELLOW_LED 27

/// Globale Preferences-Instanz für Konfigurationsspeicher
extern Preferences preferences;

// === Unterstützte Baudraten ===

/**
 * @brief Liste der erlaubten Baudraten für die serielle Kommunikation.
 *
 * Diese Baudraten können dynamisch gesetzt und vom Benutzer ausgewählt werden.
 */
constexpr int baudRates[] = {115200, 57600, 38400, 19200, 1200, 2400, 4800, 9600};

/// Anzahl der unterstützten Baudraten
constexpr size_t NUM_BAUD_RATES = sizeof(baudRates) / sizeof(baudRates[0]);

// === Access Point-Konfiguration ===

/// SSID für den Access Point
static const char *AP_SSID = "Schulz GmbH - HS-Access";

/// Passwort für den Access Point
static const char *AP_PASSWORD = "12345678";

/// Lokale IP-Adresse des Access Points
static const IPAddress AP_LOCAL_IP(192, 168, 5, 1);

/// Gateway-Adresse des Access Points
static const IPAddress AP_GATEWAY(192, 168, 5, 1);

/// Subnetzmaske des Access Points
static const IPAddress AP_SUBNET(255, 255, 255, 0);

#endif  // GLOBAL_H
