/**
 * @file StatusHandler.h
 * @brief Header-Datei für das Modul zur Anzeige von Systemstatus über LED-Muster.
 *
 * Dieses Modul definiert Statuscodes und Funktionen zur Verwaltung und Visualisierung
 * von Systemstatus mittels LEDs (z. B. auf einem ESP32). Es nutzt FreeRTOS für die zyklische
 * Ausführung und Überwachung, sowie einen Mutex zur Synchronisation des Statuspuffers.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#ifndef STATUS_HANDLER_H
#define STATUS_HANDLER_H

#include <WiFi.h>

#include <vector>

#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "global.h"

/**
 * @enum SystemStatus
 * @brief Aufzählung möglicher Systemstatus.
 *
 * Jeder Status repräsentiert einen bestimmten Betriebs- oder Fehlerzustand im System.
 * Diese Zustände werden zur LED-Anzeige genutzt.
 */
enum SystemStatus {
	SYSTEM_INITIALIZING,       ///< System wird initialisiert (grün blinkend)
	SYSTEM_READY,              ///< System betriebsbereit (grün dauerhaft)
	FS_NOT_AVAILABLE,          ///< Dateisystem nicht verfügbar (rot dauerhaft)
	LOG_NO_DIR,                ///< Log-Verzeichnis fehlt (rot blinkend, 2x)
	LOG_FILE_ERROR,            ///< Fehler beim Schreiben in Log-Datei (gelb blinkend, 2x)
	LOG_WRITE,                 ///< Schreibvorgang in Log-Datei (grün blinkend, 2x)
	WEBSERVER_NO_HTML_DIR,     ///< HTML-Verzeichnis fehlt (rot blinkend, 3x)
	WIFI_STA_NOT_AVAILABLE,    ///< Keine STA-Verbindung möglich (gelb blinkend, 3x)
	WIFI_AP_NOT_AVAILABLE,     ///< Access Point konnte nicht gestartet werden (gelb blinkend, 4x)
	WIFI_AP_NO_DEVICE,         ///< Kein Gerät am Access Point (gelb dauerhaft)
	WIFI_AP_DEVICE_AVAILABLE,  ///< Mindestens ein Gerät verbunden (gelb aus)
	SERIAL_NOT_CONNECTED,      ///< Serielle Verbindung fehlt (gelb blinkend, 1x)
	SERIAL_CONNECTED,          ///< Serielle Verbindung vorhanden (grün dauerhaft)
	SERIAL_SEND                ///< Daten werden über Serial versendet (grün blinkend, 3x)
};

/**
 * @brief Initialisiert die LED-Ausgabe und startet die Statusüberwachung.
 *
 * Konfiguriert die LED-Pins, setzt SYSTEM_INITIALIZING und startet
 * die FreeRTOS-Tasks für Statusanzeige und AP-Überwachung.
 */
void startStatusSystem();

/**
 * @brief Fügt einen Status zur aktiven Statusliste hinzu.
 *
 * Falls SYSTEM_READY gesetzt ist, wird dieser entfernt. Der Status wird
 * nur hinzugefügt, wenn er noch nicht vorhanden ist.
 *
 * @param status Der hinzuzufügende Status.
 */
void addStatus(SystemStatus status);

/**
 * @brief Entfernt einen Status aus der aktiven Statusliste.
 *
 * Nach dem Entfernen wird ggf. SYSTEM_READY gesetzt, wenn keine
 * Fehlerstatus mehr aktiv sind.
 *
 * @param status Der zu entfernende Status.
 */
void removeStatus(SystemStatus status);

/**
 * @brief Prüft, ob ein bestimmter Status derzeit aktiv ist.
 *
 * @param status Der zu prüfende Status.
 * @return true, wenn der Status aktiv ist, sonst false.
 */
bool isStatusActive(SystemStatus status);

#endif  // STATUS_HANDLER_H
