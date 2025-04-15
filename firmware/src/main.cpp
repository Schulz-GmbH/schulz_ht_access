/**
 * @file main.cpp
 * @brief Einstiegspunkt der Firmware: Initialisierung von Komponenten und Systemdiensten.
 *
 * Diese Datei enthält die zentrale `setup()`- und `loop()`-Logik für das gesamte System.
 * Beim Systemstart werden grundlegende Dienste wie Filesystem, Logging, WLAN und Webserver initialisiert.
 *
 * Im Detail:
 * - CPU-Frequenz wird auf 240 MHz gesetzt.
 * - Serielle Schnittstelle gestartet.
 * - LittleFS initialisiert.
 * - Statussystem für LED-Anzeige gestartet.
 * - Preferences geladen und ggf. formatiert.
 * - Logger aktiviert (abhängig von gespeicherter Debug-Flag).
 * - WLAN im AP+STA-Modus gestartet.
 * - Webserver (inkl. WebSocket) gestartet.
 *
 * Die `loop()`-Funktion enthält aktuell nur ein zyklisches Delay und dient als Platzhalter.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#include <LittleFS.h>
#include <global.h>

#include "FSHandler.h"
#include "LLog.h"
#include "StatusHandler.h"
#include "WebServerManager.h"
#include "WiFiManager.h"

// Globale Systeminstanzen
Preferences preferences;                ///< NVS-Konfigurationen
WiFiManager wifiManager;                ///< WLAN-Manager (AP + STA)
WebServerManager webServerManager(80);  ///< HTTP/WebSocket-Server (Port 80)

/**
 * @brief Setup-Funktion, die einmalig beim Start ausgeführt wird.
 *
 * Initialisiert das Dateisystem, das Logging, das WLAN-Modul (inkl. Access Point),
 * sowie den Webserver. Falls das NVS (Non-Volatile Storage) fehlschlägt, wird versucht,
 * es zu formatieren.
 */
void setup() {
	// CPU-Frequenz auf 240 MHz setzen
	setCpuFrequencyMhz(240);

	// Serielle Schnittstelle starten
	Serial.begin(9600);

	// LittleFS initialisieren (setzt ggf. STATUS_NO_FS)
	initFS();

	// LED-Statussystem starten (FreeRTOS-Task)
	startStatusSystem();

	// Preferences initialisieren (Namespace: "debug")
	if (!preferences.begin("debug", false)) {
		Serial.println("NVS initialisieren fehlgeschlagen, versuche NVS zu formatieren...");
		preferences.clear();
		if (!preferences.begin("system", false)) {
			Serial.println("NVS-Formatierung fehlgeschlagen.");
		}
	}

	// Debug-Flag laden (optional für spätere Nutzung)
	bool active = preferences.getBool("debug", false);

	// Logging aktivieren
	LLog::setActive(active);

	preferences.end();

	logger.info("[System] System startet...");

	// WLAN-Manager neu instanziieren (falls benötigt)
	wifiManager = WiFiManager();

	// WLAN initialisieren (AP + STA, Konfiguration aus NVS)
	wifiManager.init();

	// HTTP + WebSocket-Server initialisieren
	webServerManager.init();

	// Kurze Pause
	vTaskDelay(pdMS_TO_TICKS(1000));
}

/**
 * @brief Hauptschleife des Programms (FreeRTOS-kompatibel).
 *
 * Diese Funktion wird in der Endlosschleife ausgeführt.
 * Aktuell enthält sie nur ein Delay von 20 Sekunden, kann aber später
 * zur zyklischen Prüfung oder zum Triggern von Hintergrunddiensten genutzt werden.
 */
void loop() {
	vTaskDelay(pdMS_TO_TICKS(20000));
}
