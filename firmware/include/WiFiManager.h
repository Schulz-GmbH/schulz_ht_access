#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESPmDNS.h>
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
	String ssid;
	String password;
	int32_t rssi;
	wifi_auth_mode_t encryptionType;
	int32_t channel;
};

/**
 * @brief Struktur zur Darstellung von Scan-Ergebnissen eines WLAN-Scans.
 */
struct Network {
	String ssid;
	int32_t rssi;
	wifi_auth_mode_t encryptionType;
	int32_t channel;
};

/**
 * @class WiFiManager
 * @brief Klasse zur Verwaltung von WLAN-Funktionen (Access Point + Station).
 *
 * Unterstützt AP + STA, verwaltet gespeicherte Netzwerke sowie manuelles Verbinden,
 * Trennen, Aktivieren/Deaktivieren, Scannen und Abfragen des aktuellen Netzwerks.
 */
class WiFiManager {
   public:
	WiFiManager();

	/**
	 * @brief Initialisiert den Access Point und (optional) die Station.
	 *
	 * Startet AP per Konstante und verbindet STA automatisch, wenn aktiviert und
	 * gespeicherte Netzwerke vorhanden.
	 */
	void init();

	/**
	 * @brief Verbindet die STA mit einem gespeicherten Netzwerk.
	 * @return true bei Erfolg, false sonst.
	 */
	bool connectSaved();

	/**
	 * @brief Verbindet STA mit gegebenem SSID/Password und speichert es.
	 * @param ssid SSID des Netzwerks.
	 * @param password Passwort für SSID.
	 * @return true bei Erfolg, false sonst.
	 */
	bool connect(const String &ssid, const String &password);

	/**
	 * @brief Verbindet STA mit gegebenem SSID.
	 * Prüft ob die SSID bekannt ist und verbindet sich mit den STA mit dem Netzwerk.
	 * @param ssid SSID des Netzwerks.
	 * @return true bei Erfolg, false sonst.
	 */
	bool connect(const String &ssid);

	/**
	 * @brief Trennt die Station-Verbindung.
	 * @return true bei Erfolg, false sonst.
	 */
	bool disconnect();

	/**
	 * @brief Aktiviert die Station-Funktion.
	 */
	void activate();

	/**
	 * @brief Deaktiviert die Station-Funktion.
	 */
	void deactivate();

	/**
	 * @brief Gibt den Status der Station-Funktion zurück.
	 * @return true, wenn STA aktiviert ist, false sonst.
	 */
	bool isEnabled() const;

	bool addNetwork(const String &ssid, const String &password);
	bool removeNetwork(const String &ssid);

	/**
	 * @brief Scannt alle verfügbaren WLAN-Netzwerke.
	 * @return Vektor von Network-Strukturen.
	 */
	std::vector<Network> scan() const;

	/**
	 * @brief Gibt alle gespeicherten Netzwerke zurück.
	 * @return Vektor von WiFiNetwork.
	 */
	std::vector<WiFiNetwork> listNetworks() const;

	/**
	 * @brief Gibt das aktuell verbundene Netzwerk-SSID zurück.
	 * @return SSID oder leerer String, wenn nicht verbunden.
	 */
	String currentNetwork() const;

   private:
	Preferences config;                 ///< Preferences-Instanz
	bool enabled;                       ///< STA aktiviert
	std::vector<WiFiNetwork> networks;  ///< Gespeicherte Netzwerke

	void loadConfig();  ///< Lädt 'enabled'
	void saveConfig();  ///< Speichert 'enabled'

	void loadNetworks();  ///< Lädt Netzwerkliste (JSON)
	void saveNetworks();  ///< Speichert Netzwerkliste (JSON)

	bool existsNetwork(const String &ssid) const;
};

/// Globale Instanz
extern WiFiManager wifiManager;

#endif  // WIFI_MANAGER_H
