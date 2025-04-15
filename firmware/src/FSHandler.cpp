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
 * @brief Initialisiert das LittleFS-Dateisystem und überprüft die erforderlichen Verzeichnisse.
 *
 * Diese Funktion versucht, das LittleFS-Dateisystem zu initialisieren. Wird es nicht
 * erkannt, wird der Fehlerstatus STATUS_NO_SD_CARD gesetzt und die Status für fehlende
 * Verzeichnisse (WEBSERVER_NO_HTML_DIR, LOG_NO_DIR) entfernt. Ist die Initialisierung
 * erfolgreich, wird geprüft, ob die Verzeichnisse vorhanden sind.
 *
 * Fehlt eines der Verzeichnisse, wird der entsprechende Fehlerstatus gesetzt.
 * Sind beide Verzeichnisse vorhanden, so wird SYSTEM_READY aktiviert.
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
 * @brief Überprüft regelmäßig den Zustand des Dateisystems und der erforderlichen Verzeichnisse.
 *
 * Diese Funktion wird z. B. in der loop()-Funktion aufgerufen, um den aktuellen
 * Zustand des Dateisystems zu überwachen. Dabei wird erneut versucht,
 * LittleFS zu initialisieren. Ist es nicht mehr verfügbar, wird STATUS_NO_SD_CARD
 * gesetzt und die anderen Verzeichnis-Status entfernt.
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
