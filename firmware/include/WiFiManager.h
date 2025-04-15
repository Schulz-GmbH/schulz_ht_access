/**
 * @file WiFiManager.h
 * @brief Header-Datei für das WLAN-Management-Modul.
 *
 * Diese Datei deklariert die `WiFiManager`-Klasse, die den AP- und STA-Betrieb des ESP32 verwaltet.
 * Zusätzlich bietet sie Methoden zur Konfiguration, Verbindung und Verwaltung gespeicherter Netzwerke.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

#include <vector>

#include "LLog.h"
#include "StatusHandler.h"
#include "global.h"

/**
 * @brief Struktur zur Repräsentation eines gespeicherten WLAN-Netzwerks.
 */
struct WiFiNetwork {
	String ssid;      ///< SSID des Netzwerks
	String password;  ///< Passwort des Netzwerks
};

/**
 * @brief Struktur zur Darstellung von Scan-Ergebnissen eines WLAN-Scans.
 */
struct Network {
	String ssid;             ///< SSID des Netzwerks
	int32_t rssi;            ///< Signalstärke in dBm
	uint8_t encryptionType;  ///< Verschlüsselungstyp
	int32_t channel;         ///< Kanalnummer
};

/**
 * @class WiFiManager
 * @brief Klasse zur Verwaltung von WLAN-Funktionen (Access Point + Station).
 */
class WiFiManager {
   public:
	WiFiManager();

	/// Initialisiert den WLAN-Modus (AP+STA) und stellt Verbindung zu Netzwerken her.
	void init();

	/// Liefert die aktuell gespeicherte SSID zurück.
	void getNetwork(String &ssid);

	/// Verbindet automatisch mit bekannten WLANs.
	wl_status_t connect();

	/// Verbindet explizit mit einem bestimmten Netzwerk.
	wl_status_t connect(const String ssid, const String password);

	/// Trennt die aktuelle Verbindung.
	bool disconnect();

	/// Fügt ein neues WLAN zur gespeicherten Liste hinzu.
	void addNetwork(const String ssid, const String password);

	/// Entfernt ein WLAN anhand der SSID.
	bool removeNetwork(const String ssid);

	/// Gibt alle gespeicherten Netzwerke zurück.
	std::vector<WiFiNetwork> listNetwork();

	/// Führt einen WLAN-Scan durch und liefert die Ergebnisse.
	std::vector<Network> scan();

   private:
	Preferences config;  ///< Preferences-Instanz für Konfigurationsdaten
	String ssid;         ///< Aktuelle SSID (STA-Modus)
	String password;     ///< Passwort für STA-Netzwerk
	bool enabled;        ///< WLAN aktiviert oder deaktiviert

	std::vector<WiFiNetwork> networks;  ///< Liste gespeicherter Netzwerke

	/// Lädt STA-Konfiguration (SSID, Passwort, Status)
	void loadConfig();

	/// Speichert aktuelle STA-Konfiguration
	void saveConfig();

	/// Lädt gespeicherte Netzwerke aus Preferences
	void loadNetworks();

	/// Speichert gespeicherte Netzwerke in Preferences
	void saveNetworks();

	/// Prüft, ob ein Netzwerk mit gegebener SSID bereits existiert
	bool existsNetwork(const String &ssid);
};

/// Globale Instanz
extern WiFiManager wifiManager;

#endif  // WIFI_MANAGER_H
