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
#include <nvs.h>
#include <nvs_flash.h>

#include "FSHandler.h"
#include "LLog.h"
#include "SerialBridge.h"
#include "StatusHandler.h"
#include "WebServerManager.h"
#include "WebSocketManager.h"
#include "WiFiManager.h"

// === Globale Systeminstanzen ===

/// NVS-Konfigurationen
Preferences preferences;

/// WLAN-Manager für AP+STA-Betrieb
WiFiManager wifiManager;

/// HTTP-Server (statisch), verwaltet statische Inhalte und REST-Routen
WebServerManager webServerManager;

/// WebSocket-Manager für Echtzeitkommunikation mit dem Client
WebSocketManager webSocketManager("/ws");

/// Task-Handle für die SerialBridge
static TaskHandle_t serialBridgeTaskHandle = nullptr;

/// Globale SerialBridge-Instanz zur Kommunikation über UART2
SerialBridge* serialBridge = nullptr;

/**
 * @brief Gibt den Inhalt eines Verzeichnisses rekursiv im Log aus.
 *
 * Verwendet LittleFS zur Auflistung von Dateien und Verzeichnissen.
 *
 * @param dirname Pfad des Wurzelverzeichnisses.
 * @param depth Aktuelle Einrücktiefe (für rekursive Aufrufe).
 */
void listDir(const char* dirname, uint8_t depth = 0) {
	// Prefix für Einrückung
	String indent;
	for (uint8_t i = 0; i < depth; ++i) indent += "  ";

	// Log-Ausgabe für das aktuelle Verzeichnis
	logger.log({"system", "info", "filesystem"}, indent + "Verzeichnis: " + String(dirname));

	File dir = LittleFS.open(dirname);
	if (!dir) {
		logger.log({"system", "error", "filesystem"}, indent + "  Öffnen fehlgeschlagen");
		return;
	}
	if (!dir.isDirectory()) {
		logger.log({"system", "error", "filesystem"}, indent + "  Kein Verzeichnis");
		dir.close();
		return;
	}

	File entry;
	while ((entry = dir.openNextFile())) {
		String name = String(entry.name());
		bool isDir = entry.isDirectory();
		String detail = indent + "  ";
		detail += isDir ? "[DIR]  " : "[FILE] ";
		detail += name;
		if (!isDir) {
			detail += " (" + String(entry.size()) + " bytes)";
		}
		// Pro Eintrag loggen
		logger.log({"system", "info", "filesystem"}, detail);

		// Wenn es ein Unterverzeichnis ist, rekursiv hinein
		if (isDir) {
			// Pfad zusammenbauen: dirname + "/" + Unterverzeichnis
			String sub = String(dirname);
			if (!sub.endsWith("/")) sub += "/";
			sub += name;
			listDir(sub.c_str(), depth + 1);
		}

		entry.close();
	}
	dir.close();
}

/**
 * @brief Gibt alle gespeicherten NVS-Werte in einem Namespace aus.
 *
 * Unterstützt die Typen: `NVS_TYPE_STR`, `NVS_TYPE_U8`, `NVS_TYPE_I32`.
 *
 * @param ns Name des NVS-Namespaces.
 */
static void dumpNamespace(const char* ns) {
	// Stelle sicher, dass NVS initialisiert ist
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		nvs_flash_erase();
		nvs_flash_init();
	}
	nvs_handle_t h;
	if ((err = nvs_open(ns, NVS_READONLY, &h)) != ESP_OK) {
		Serial.printf("NVS öffnen fehlgeschlagen: %s (%d)\n", ns, err);
		return;
	}

	// Iterator über alle Einträge im Namespace
	nvs_iterator_t it = nvs_entry_find(NVS_DEFAULT_PART_NAME, ns, NVS_TYPE_ANY);
	while (it) {
		nvs_entry_info_t info;
		nvs_entry_info(it, &info);
		Serial.printf("Key: %s  Type: %d", info.key, info.type);

		// Je nach Typ auslesen
		if (info.type == NVS_TYPE_STR) {
			size_t len = 0;
			nvs_get_str(h, info.key, nullptr, &len);
			char* buf = (char*)malloc(len);
			nvs_get_str(h, info.key, buf, &len);
			Serial.printf("  Value: \"%s\"\n", buf);
			free(buf);
		} else if (info.type == NVS_TYPE_U8) {
			uint8_t v;
			nvs_get_u8(h, info.key, &v);
			Serial.printf("  Value: %u\n", v);
		} else if (info.type == NVS_TYPE_I32) {
			int32_t v;
			nvs_get_i32(h, info.key, &v);
			Serial.printf("  Value: %ld\n", v);
		} else {
			Serial.println();
		}

		it = nvs_entry_next(it);
	}
	nvs_release_iterator(it);
	nvs_close(h);
}

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
		preferences.clear();
		preferences.begin("debug", false);
	}

	// Debug-Flag laden (optional für spätere Nutzung)
	bool fileLog = preferences.getBool("fileLogging", false);
	// Logging aktivieren/deaktivieren
	LLog::setFileLogging(fileLog);
	preferences.end();

	logger.log({"system", "info"}, "System startet...");

	// WLAN initialisieren (AP + STA, Konfiguration aus NVS)
	wifiManager.init();
	// HTTP- und WebSocket-Server initialisieren
	static AsyncWebServer server(80);
	webServerManager.init(server);  // HTTP-Routen
	webSocketManager.init(server);  // WS-Routen
	server.begin();
	logger.log({"system", "info"}, "HTTP & WS gestartet");

	// SerialBridge über WS
	serialBridge = new SerialBridge(Serial2, webSocketManager.getSocket(), RXD2, TXD2);
	serialBridge->begin(9600);
	serialBridge->start(&serialBridgeTaskHandle, 3, 1);
	logger.log({"system", "info", "device"}, "UART2 gestartet auf RX=16, TX=17, 9600 Baud");

	// Kurze Pause
	vTaskDelay(pdMS_TO_TICKS(1000));

	if (logger.isFileLogging()) {
		Serial.println("=== FS Debug ===");
		listDir("/");  // Root-Verzeichnis
		Serial.println("===============\n");

		Serial.println("=== Dump wifi_config NVS ===");
		dumpNamespace("wifi_config");
		Serial.println("=== Ende Dump ===");
	}
}

/**
 * @brief Hauptschleife des Programms (FreeRTOS-kompatibel).
 *
 * Diese Funktion wird in der Endlosschleife ausgeführt.
 * Aktuell enthält sie nur ein Delay von 20 Sekunden, kann aber später
 * zur zyklischen Prüfung oder zum Triggern von Hintergrunddiensten genutzt werden.
 */
void loop() {
	// Alle 500 ms testen, ob die Bridge noch lebt
	vTaskDelay(pdMS_TO_TICKS(500));
}
