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
struct ScannedNetwork {
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

	/**
	 * @brief Setzt die STA-Konfiguration und speichert sie.
	 *
	 * Diese Methode aktualisiert die WLAN-Zugangsdaten (SSID und Passwort) und speichert
	 * diese in den Preferences.
	 *
	 * @param ssid Die SSID des WLANs.
	 * @param password Das Passwort des WLANs.
	 */
	void setSTAConfig(const String &ssid, const String &password);

	/**
	 * @brief Gibt die aktuelle STA-Konfiguration zurück.
	 *
	 * Diese Methode liefert die aktuell gespeicherten WLAN-Zugangsdaten.
	 *
	 * @param ssid Referenzvariable zur Rückgabe der SSID.
	 * @param password Referenzvariable zur Rückgabe des Passworts.
	 */
	void getSTAConfig(String &ssid, String &password);

	/**
	 * @brief Gibt alle in der internen Liste gespeicherten Netzwerke zurück.
	 *
	 * @return Vektor mit den gespeicherten WLAN-Netzwerken.
	 */
	std::vector<WiFiNetwork> getSavedNetworks();

	/**
	 * @brief Fügt ein neues WLAN-Netz zur Liste der gespeicherten Netzwerke hinzu.
	 *
	 * Diese Methode überprüft, ob das angegebene Netzwerk bereits in der Liste vorhanden ist.
	 * Falls nicht, wird es hinzugefügt.
	 *
	 * @param ssid Die SSID des neuen WLAN-Netzwerks.
	 * @param password Das Passwort des neuen WLAN-Netzwerks.
	 */
	void addNetwork(const String &ssid, const String &password);

	/**
	 * @brief Entfernt ein gespeichertes WLAN-Netz aus der Liste.
	 *
	 * Diese Methode sucht nach dem Netzwerk anhand der SSID und entfernt es, sofern vorhanden.
	 *
	 * @param ssid Die SSID des zu entfernenden Netzwerks.
	 * @return true, falls das Netzwerk entfernt wurde; andernfalls false.
	 */
	bool removeNetwork(const String &ssid);

	/**
	 * @brief Versucht, eine STA-Verbindung herzustellen.
	 *
	 * Zunächst wird versucht, über die gespeicherte STA-Konfiguration eine Verbindung herzustellen.
	 * Scheitert dies, wird ein WLAN-Scan durchgeführt und geprüft, ob eines der gespeicherten Netzwerke
	 * im Scan-Ergebnis vorhanden ist. Wird ein bekanntes Netzwerk gefunden, wird versucht, sich damit zu verbinden.
	 * Bei erfolgreicher Verbindung wird die STA-Konfiguration aktualisiert.
	 *
	 * @return true, wenn eine Verbindung erfolgreich hergestellt wurde, sonst false.
	 */
	bool connectSTA();

	/**
	 * @brief Trennt die aktuelle STA-Verbindung.
	 *
	 * Führt einen sauberen Disconnect des STA-Modus durch und loggt die Trennung.
	 */
	void disconnectSTA();

	/**
	 * @brief Führt einen Scan der verfügbaren WLAN-Netzwerke durch.
	 *
	 * Diese Methode startet einen WLAN-Scan und sammelt die Ergebnisse in einem Vektor von ScannedNetwork-Strukturen.
	 *
	 * @return Vektor mit den gefundenen Netzwerken.
	 */
	std::vector<ScannedNetwork> scanNetworks();

   private:
	// STA-Konfiguration (wird persistiert)
	String staSSID;
	String staPassword;

	// Liste der gespeicherten Netzwerke (diese kann zusätzlich persistiert werden, falls gewünscht)
	std::vector<WiFiNetwork> savedNetworks;

	// Preferences-Instanz für persistente Speicherung (als Klassenmember)
	Preferences staPreferences;   // für STA-Daten ("wifi_sta")
	Preferences netsPreferences;  // für Netzwerkliste ("wifi_nets")

	// Methoden zum Laden und Speichern der STA-Konfiguration
	void loadConfig();
	void saveConfig();
	void loadSavedNetworks();
	void saveSavedNetworks();
};

#endif  // WIFI_MANAGER_H
