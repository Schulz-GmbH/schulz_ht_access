/**
 * @file FSHandler.cpp
 * @brief Modul zur Initialisierung und Überwachung des LittleFS-Dateisystems.
 *
 * Dieses Modul übernimmt die Initialisierung von LittleFS und prüft, ob
 * die erforderlichen Verzeichnisse "/www/html" und "/logs" vorhanden sind.
 * Bei Fehlern werden entsprechende Status (STATUS_NO_SD_CARD, WEBSERVER_NO_HTML_DIR,
 * LOG_NO_DIR) gesetzt bzw. entfernt. Sind alle Verzeichnisse vorhanden,
 * wird SYSTEM_READY aktiviert.
 *
 * Die Funktion initFS() führt eine einmalige Initialisierung und Überprüfung durch,
 * während checkFS() zur regelmäßigen Überwachung (z. B. in der loop()-Funktion) verwendet wird.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#include "FSHandler.h"

/**
 * @brief Überprüft die erforderlichen Verzeichnisse im LittleFS.
 *
 * Diese Funktion prüft, ob die Verzeichnisse "/www/html" und "/logs" vorhanden sind.
 * Bei fehlenden Verzeichnissen werden die entsprechenden Fehlerstatus gesetzt.
 *
 * @return true, wenn mindestens ein Verzeichnis fehlt, andernfalls false.
 */
static bool checkDirectories() {
	bool missingAnything = false;

	if (!LittleFS.exists("/www/html")) {
		logger.error("[LittleFS] Verzeichnis /www/html fehlt im Dateisystem!");
		addStatus(WEBSERVER_NO_HTML_DIR);
		missingAnything = true;
	} else {
		removeStatus(WEBSERVER_NO_HTML_DIR);
	}

	if (!LittleFS.exists("/logs")) {
		logger.error("[LittleFS] Verzeichnis /logs fehlt im Dateisystem!");
		addStatus(LOG_NO_DIR);
		missingAnything = true;
	} else {
		removeStatus(LOG_NO_DIR);
	}

	return missingAnything;
}

/**
 * @brief Initialisiert das LittleFS-Dateisystem.
 *
 * Diese Funktion wird einmalig beim Systemstart aufgerufen. Sie mountet das Dateisystem,
 * erstellt ggf. die erforderlichen Verzeichnisse (z. B. `/www/html`, `/logs`)
 * und aktualisiert die entsprechenden Statusflags.
 *
 * Bei erfolgreicher Initialisierung und Verzeichnisprüfung wird `SYSTEM_READY` gesetzt.
 * Im Fehlerfall werden `FS_NOT_AVAILABLE`, `WEBSERVER_NO_HTML_DIR` oder `LOG_NO_DIR` gesetzt.
 */
void initFS() {
	bool fsOk = LittleFS.begin();

	if (!fsOk) {
		logger.error("[LittleFS] Dateisystem konnte nicht initialisiert werden!");
		addStatus(FS_NOT_AVAILABLE);
		removeStatus(WEBSERVER_NO_HTML_DIR);
		removeStatus(LOG_NO_DIR);
		removeStatus(SYSTEM_INITIALIZING);
		return;
	} else {
		removeStatus(FS_NOT_AVAILABLE);
		logger.info("[LittleFS] Dateisystem erfolgreich initialisiert.");
	}

	// Verzeichnisse anlegen, falls nicht vorhanden:
	if (!LittleFS.exists("/www/html")) {
		LittleFS.mkdir("/www/html");
	}
	if (!LittleFS.exists("/logs")) {
		LittleFS.mkdir("/logs");
	}

	removeStatus(SYSTEM_INITIALIZING);

	if (!checkDirectories()) {
		addStatus(SYSTEM_READY);
	} else {
		removeStatus(SYSTEM_READY);
	}
}

/**
 * @brief Führt eine zyklische Prüfung des Dateisystems durch.
 *
 * Diese Funktion sollte regelmäßig (z. B. in der `loop()`-Funktion oder einem FreeRTOS-Task)
 * aufgerufen werden, um sicherzustellen, dass das Dateisystem weiterhin verfügbar ist
 * und alle erforderlichen Verzeichnisse existieren.
 *
 * Erkennt Änderungen und aktualisiert die Statusflags entsprechend.
 */
void checkFS() {
	bool fsOk = LittleFS.begin();

	if (!fsOk) {
		logger.error("[LittleFS] Dateisystem nicht verfügbar!");
		addStatus(FS_NOT_AVAILABLE);
		removeStatus(WEBSERVER_NO_HTML_DIR);
		removeStatus(LOG_NO_DIR);
		return;
	} else {
		removeStatus(FS_NOT_AVAILABLE);
	}

	if (!checkDirectories()) {
		addStatus(SYSTEM_READY);
	} else {
		removeStatus(SYSTEM_READY);
	}
}
