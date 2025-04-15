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
 * @brief Struktur zur Darstellung eines gespeicherten WLAN-Netzwerks.
 */
struct WiFiNetwork {
	String ssid;      ///< SSID des WLANs
	String password;  ///< Passwort des WLANs
};

/**
 * @brief Struktur zur Darstellung von Scan-Ergebnissen.
 */
struct Network {
	String ssid;             ///< SSID des gefundenen Netzwerks
	int32_t rssi;            ///< Signalstärke des Netzwerks (RSSI)
	uint8_t encryptionType;  ///< Verschlüsselungstyp des Netzwerks
	int32_t channel;         ///< Kanal des Netzwerks
};

/**
 * @brief Klasse zur Verwaltung der WLAN-Funktionalitäten (AP + STA).
 *
 * Diese Klasse bietet Funktionen zur Verwaltung der WLAN-Konfiguration,
 * zum Laden und Speichern der STA-Konfiguration sowie der Liste gespeicherter Netzwerke.
 * Zudem werden Funktionen zum Initialisieren des WLANs, zum Verbinden im STA-Modus,
 * zum Durchführen von Scans und zum Trennen der Verbindung bereitgestellt.
 */
class WiFiManager {
   public:
	/**
	 * @brief Konstruktor für den WiFiManager.
	 *
	 * Initialisiert die WiFiManager-Instanz und lädt sowohl die gespeicherte STA-Konfiguration
	 * als auch die Liste der gespeicherten Netzwerke.
	 */
	WiFiManager();

	/**
	 * @brief Initialisiert das WLAN im AP+STA-Modus.
	 *
	 * Diese Methode setzt den WiFi-Modus auf AP+STA, konfiguriert und startet den Access Point
	 * (mit festen Zugangsdaten) und versucht, eine Verbindung als Station (STA) herzustellen,
	 * sofern eine STA-Konfiguration vorhanden ist.
	 */
	void init();
	void getNetwork(String &ssid);

	wl_status_t connect();
	wl_status_t connect(const String ssid, const String password);
	bool disconnect();

	void addNetwork(const String ssid, const String password);
	bool removeNetwork(const String ssid);
	std::vector<WiFiNetwork> listNetwork();
	std::vector<Network> scan();

   private:
	Preferences config;
	String ssid;
	String password;
	bool enabled;

	std::vector<WiFiNetwork> networks;

	void loadConfig();
	void saveConfig();

	void loadNetworks();
	void saveNetworks();

	bool existsNetwork(const String &ssid);
};

extern WiFiManager wifiManager;

#endif  // WIFI_MANAGER_H
