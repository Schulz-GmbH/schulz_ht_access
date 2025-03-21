/**
 * @file SDCard.cpp
 * @brief Modul zur Initialisierung und Überwachung der SD-Karte.
 *
 * Dieses Modul übernimmt die Initialisierung der SD-Karte und prüft, ob
 * die erforderlichen Verzeichnisse "/www/html" und "/logs" vorhanden sind.
 * Bei Fehlern werden entsprechende Status (STATUS_NO_SD_CARD, STATUS_NO_HTML_DIR,
 * STATUS_NO_LOGS_DIR) gesetzt bzw. entfernt. Sind alle Verzeichnisse vorhanden,
 * wird STATUS_READY aktiviert.
 *
 * Die Funktion initSDCard() führt eine einmalige Initialisierung und Überprüfung der
 * SD-Karte durch, während checkSDCard() zur regelmäßigen Überwachung (z. B. in der
 * loop()-Funktion) verwendet wird.
 *
 * @author Simon MArcel Linden
 * @since 1.0.0
 */

#include "SDCard.h"

/**
 * @brief Überprüft die erforderlichen Verzeichnisse auf der SD-Karte.
 *
 * Diese Funktion prüft, ob das Verzeichnis "/www/html" und "/logs" vorhanden sind.
 * Bei fehlenden Verzeichnissen werden die entsprechenden Fehlerstatus gesetzt.
 *
 * @return true, wenn mindestens ein Verzeichnis fehlt, andernfalls false.
 */
static bool checkDirectories() {
	bool missingAnything = false;

	// Überprüfung des Verzeichnisses /www/html
	if (!SD.exists("/www/html")) {
		logger.error("[SD-CARD] Verzeichnis /www/html fehlt auf der SD-Karte!");
		addStatus(STATUS_NO_HTML_DIR);
		missingAnything = true;
	} else {
		removeStatus(STATUS_NO_HTML_DIR);
	}

	// Überprüfung des Verzeichnisses /logs
	if (!SD.exists("/logs")) {
		logger.error("[SD-CARD] Verzeichnis /logs fehlt auf der SD-Karte!");
		addStatus(STATUS_NO_LOGS_DIR);
		missingAnything = true;
	} else {
		removeStatus(STATUS_NO_LOGS_DIR);
	}

	return missingAnything;
}

/**
 * @brief Initialisiert die SD-Karte und überprüft die erforderlichen Verzeichnisse.
 *
 * Diese Funktion versucht, die SD-Karte zu initialisieren. Wird die SD-Karte nicht
 * erkannt, wird der Fehlerstatus STATUS_NO_SD_CARD gesetzt und die Status für fehlende
 * Verzeichnisse (STATUS_NO_HTML_DIR, STATUS_NO_LOGS_DIR) entfernt. Ist die Initialisierung
 * erfolgreich, wird überprüft, ob die Verzeichnisse "/www/html" und "/logs" vorhanden sind.
 * Fehlt eines der Verzeichnisse, wird der entsprechende Fehlerstatus gesetzt. Sind beide
 * Verzeichnisse vorhanden, so wird STATUS_READY aktiviert.
 */
void initSDCard() {
	bool sdCardOk = SD.begin(SD_CS_PIN);

	if (!sdCardOk) {
		logger.error("[SD-CARD] SD-Karte konnte nicht initialisiert werden!");
		addStatus(STATUS_NO_SD_CARD);

		removeStatus(STATUS_NO_HTML_DIR);
		removeStatus(STATUS_NO_LOGS_DIR);
		// Entferne STATUS_INITIALIZING, da die Initialisierung abgeschlossen ist
		removeStatus(STATUS_INITIALIZING);

		return;
	} else {
		removeStatus(STATUS_NO_SD_CARD);
		logger.info("[SD-CARD] SD-Karte erfolgreich initialisiert.");
	}

	// Entferne den Initialisierungsstatus, da die Überprüfung abgeschlossen ist
	removeStatus(STATUS_INITIALIZING);

	// Falls alle Verzeichnisse vorhanden sind, wird STATUS_READY gesetzt
	if (!checkDirectories()) {
		addStatus(STATUS_READY);
	} else {
		removeStatus(STATUS_READY);
	}
}

/**
 * @brief Überprüft regelmäßig den Zustand der SD-Karte und der erforderlichen Verzeichnisse.
 *
 * Diese Funktion wird beispielsweise in der loop()-Funktion aufgerufen, um den aktuellen
 * Zustand der SD-Karte zu überwachen. Dabei wird erneut versucht, die SD-Karte zu initialisieren.
 * Ist die SD-Karte nicht mehr verfügbar, wird STATUS_NO_SD_CARD gesetzt und die anderen
 * Verzeichnis-Status entfernt. Andernfalls werden die Verzeichnisse "/www/html" und "/logs"
 * überprüft. Fehlende Verzeichnisse führen zur Setzung der entsprechenden Fehlerstatus.
 */

void checkSDCard() {
	// Einfachheitshalber wird hier erneut SD.begin() aufgerufen.
	bool sdCardOk = SD.begin(SD_CS_PIN);
	if (!sdCardOk) {
		// SD-Karte ist entfernt oder nicht mehr erreichbar
		logger.error("[SD-CARD] SD-Karte nicht verfügbar!");
		addStatus(STATUS_NO_SD_CARD);
		// Die Überprüfung der Verzeichnisse macht keinen Sinn, daher werden diese Status entfernt
		removeStatus(STATUS_NO_HTML_DIR);
		removeStatus(STATUS_NO_LOGS_DIR);

		return;
	} else {
		// SD-Karte ist noch vorhanden
		removeStatus(STATUS_NO_SD_CARD);
	}

	// Falls alle Verzeichnisse vorhanden sind, wird STATUS_READY gesetzt
	if (!checkDirectories()) {
		addStatus(STATUS_READY);
	} else {
		removeStatus(STATUS_READY);
	}
}
