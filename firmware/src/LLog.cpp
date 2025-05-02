/**
 * @file LLog.cpp
 * @brief Singleton-Modul zur Verwaltung von systemweitem Logging auf LittleFS und serielle Schnittstelle.
 *
 * Das LLog-Modul stellt eine zentrale, einfache Möglichkeit bereit, Log-Ausgaben strukturiert
 * und einheitlich zu erstellen. Dabei werden Logeinträge sowohl auf der seriellen Schnittstelle
 * ausgegeben als auch dauerhaft in einer Logdatei im Flash-Dateisystem (LittleFS) gespeichert.
 *
 * Unterstützte Log-Level:
 * - DEBUG
 * - INFO
 * - SYSTEM
 * - WARNING
 * - ERROR
 *
 * Das Modul unterstützt zudem das dynamische Aktivieren und Deaktivieren des Loggings. Beim Aktivieren
 * des Loggings wird eine neue Logdatei mit Zeitstempel erstellt. Ist keine gültige Uhrzeit vorhanden
 * (z. B. RTC nicht verfügbar), wird stattdessen eine eindeutige ID als Dateiname verwendet.
 *
 * Durch Nutzung des Singleton-Patterns ist eine systemweite, konsistente Nutzung gewährleistet.
 *
 * Zusätzlich bietet das Modul flexible Methoden zum Loggen mit oder ohne Zeilenumbruch und optionalem Zeitstempel.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#include "LLog.h"

#include "global.h"

bool LLog::m_fileLogging = true;
const std::vector<String> LLog::Events = {"debug", "info", "system", "warning", "error", "socket", "http", "general"};
/**
 * @brief Konstruktor des LLog-Singletons.
 *
 * Initialisiert das Logging-System, mountet LittleFS und erstellt ggf. die notwendigen
 * Verzeichnisse zur Speicherung der Log-Dateien.
 */
LLog::LLog() {
	if (!LittleFS.begin()) {
		Serial.println("[LLog] Fehler: LittleFS konnte nicht gemountet werden.");
	}
	// Verzeichnisse anlegen
	if (!LittleFS.exists("/logs")) {
		LittleFS.mkdir("/logs");
	}
	if (!LittleFS.exists("/logs/system")) {
		LittleFS.mkdir("/logs/system");
	}
}

/**
 * @brief Gibt die Singleton-Instanz von LLog zurück.
 *
 * @return Referenz auf die einzige LLog-Instanz.
 */
LLog &LLog::getInstance() {
	static LLog instance;
	return instance;
}

/**
 * @brief Erzeugt einen formatierten Zeitstempel im Format YYYY-MM-DD HH:MM:SS.
 *
 * @return Zeitstempel als String.
 */
String LLog::getTimestamp() {
	time_t now = time(nullptr);
	struct tm timeinfo;
	localtime_r(&now, &timeinfo);
	char buf[20];
	snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour,
	         timeinfo.tm_min, timeinfo.tm_sec);
	return String(buf);
}

/**
 * @brief Interne Methode zur Protokollierung einer Log-Nachricht.
 *
 * Diese Methode gibt eine Logzeile sowohl auf der seriellen Schnittstelle aus
 * als auch in die entsprechende Datei im Dateisystem, abhängig vom Log-Level.
 *
 * @param level Der Log-Level als String (z. B. "[INFO]").
 * @param message Die zu loggende Nachricht.
 * @param newLine true, um einen Zeilenumbruch am Ende einzufügen.
 * @param timestamp true, um einen Zeitstempel in die Logzeile einzufügen.
 */
void LLog::logMessage(const char *level, const String &message, bool newLine, bool timestamp) {
	// 1) Baue Logzeile mit optionalem Zeitstempel
	String entry;
	if (timestamp) {
		entry = String(level) + " [" + getTimestamp() + "] " + message;
	} else {
		entry = String(level) + " " + message;
	}

	// 2) Auf Serial ausgeben
	if (newLine) {
		Serial.println(entry);
	} else {
		Serial.print(entry);
	}

	// 3) In Datei speichern
	String fname = "general.log";
	// Level-String ohne [] und lowercase
	String lvl = String(level);
	lvl.replace("[", "");
	lvl.replace("]", "");
	lvl.toLowerCase();
	auto it = std::find(Events.begin(), Events.end(), lvl);
	if (it != Events.end()) {
		fname = *it + ".log";
	}

	logToFile(fname, entry);
}

void LLog::logMessage(const std::vector<String> &levels, const String &message, bool newLine, bool timestamp) {
	String prefix;
	for (const auto &evt : levels) {
		String cap = evt;
		for (size_t i = 0; i < cap.length(); ++i) {
			cap[i] = toupper(cap[i]);
		}
		prefix += "[" + cap + "]";
	}

	// 1) Baue Logzeile mit optionalem Zeitstempel
	String entry;
	if (timestamp) {
		entry = prefix + "[" + getTimestamp() + "] " + message;
	} else {
		entry = prefix + " " + message;
	}

	// 2) Auf Serial ausgeben
	if (newLine) {
		Serial.println(entry);
	} else {
		Serial.print(entry);
	}

	for (const auto &evt : levels) {
		// 3) In Datei speichern
		String fname = "general.log";
		// Level-String ohne [] und lowercase
		String lvl = String(evt);
		lvl.replace("[", "");
		lvl.replace("]", "");
		lvl.toLowerCase();
		auto it = std::find(Events.begin(), Events.end(), lvl);
		if (it != Events.end()) {
			fname = *it + ".log";
		}

		logToFile(fname, entry);
	}
}

/**
 * @brief Schreibt einen Log-Eintrag in die angegebene Datei im LittleFS-Dateisystem.
 *
 * @param filename Dateiname der Log-Datei (relativ zu /logs/system/).
 * @param entry Der zu speichernde Log-Eintrag.
 */
void LLog::logToFile(const String &filename, const String &entry) {
	if (!m_fileLogging) return;
	String path = "/logs/system/" + filename;
	File f = LittleFS.open(path, FILE_APPEND);
	if (!f) {
		File t = LittleFS.open(path, FILE_WRITE);
		if (t) t.close();
		f = LittleFS.open(path, FILE_APPEND);
	}
	if (f) {
		f.println(entry);
		f.close();
	} else {
		Serial.printf("[LLog] Fehler beim Öffnen von %s\n", path.c_str());
	}
}

/**
 * @brief Loggt eine Debug-Nachricht.
 *
 * @param message Die zu loggende Nachricht.
 * @param newLine true, um einen Zeilenumbruch hinzuzufügen (Standard: true).
 */
void LLog::debug(const String &message, bool newLine) {
	logMessage("[DEBUG]", message, newLine, true);
}

/**
 * @brief Loggt eine Info-Nachricht.
 *
 * @param message Die zu loggende Nachricht.
 * @param newLine true, um einen Zeilenumbruch hinzuzufügen (Standard: true).
 */
void LLog::info(const String &message, bool newLine) {
	logMessage("[INFO]", message, newLine, true);
}

/**
 * @brief Loggt eine System-Nachricht.
 *
 * @param message Die zu loggende Nachricht.
 * @param newLine true, um einen Zeilenumbruch hinzuzufügen (Standard: true).
 */
void LLog::system(const String &message, bool newLine) {
	logMessage("[SYSTEM]", message, newLine, true);
}

/**
 * @brief Loggt eine Warnung.
 *
 * @param message Die zu loggende Nachricht.
 * @param newLine true, um einen Zeilenumbruch hinzuzufügen (Standard: true).
 */
void LLog::warn(const String &message, bool newLine) {
	logMessage("[WARNING]", message, newLine, true);
}

/**
 * @brief Loggt einen Fehler.
 *
 * @param message Die zu loggende Nachricht.
 * @param newLine true, um einen Zeilenumbruch hinzuzufügen (Standard: true).
 */
void LLog::error(const String &message, bool newLine) {
	logMessage("[ERROR]", message, newLine, true);
}

/**
 * @brief Loggt ein Socket Event.
 *
 * @param message Die zu loggende Nachricht.
 * @param newLine true, um einen Zeilenumbruch hinzuzufügen (Standard: true).
 */
void LLog::socket(const String &message, bool newLine) {
	logMessage("[Socket]", message, newLine, true);
}

/**
 * @brief Loggt ein HTTP Event.
 *
 * @param message Die zu loggende Nachricht.
 * @param newLine true, um einen Zeilenumbruch hinzuzufügen (Standard: true).
 */
void LLog::http(const String &message, bool newLine) {
	logMessage("[HTTP]", message, newLine, true);
}

/**
 * @brief Loggt ein Fehler aus mehreren Events.
 *
 * @param events z.B. {"warning","socket"}
 * @param message Die zu loggende Nachricht.
 * @param newLine true, um einen Zeilenumbruch hinzuzufügen (Standard: true).
 */
void LLog::log(const std::vector<String> &events, const String &message, bool newLine) {
	// Kombiniertes Loggen in einer einzigen Zeile
	// String prefix;
	// for (const auto &evt : events) {
	// 	prefix += "[" + evt + "]";
	// }
	// Einmaliges Loggen mit gesamtem Prefix
	// logMessage(prefix.c_str(), message, newLine, true);
	logMessage(events, message, newLine, true);
}

/**
 * @brief Gibt eine Nachricht ohne Zeilenumbruch auf der seriellen Schnittstelle aus.
 *
 * Optional kann ein Zeitstempel mitgeliefert werden. Diese Nachricht wird nicht geloggt.
 *
 * @param message Die auszugebende Nachricht.
 * @param timestamp true, um einen Zeitstempel voranzustellen.
 */
void LLog::print(const String &message, bool timestamp) {
	logMessage("", message, false, timestamp);
}

/**
 * @brief Gibt eine Nachricht mit Zeilenumbruch auf der seriellen Schnittstelle aus.
 *
 * Optional kann ein Zeitstempel mitgeliefert werden. Diese Nachricht wird nicht geloggt.
 *
 * @param message Die auszugebende Nachricht.
 * @param timestamp true, um einen Zeitstempel voranzustellen.
 */
void LLog::println(const String &message, bool timestamp) {
	logMessage("", message, true, timestamp);
}

/**
 * @brief Aktiviert oder deaktiviert das Schreiben in Log-Dateien.
 *
 * Der Zustand wird persistent in den Preferences unter `debug/fileLogging` gespeichert.
 *
 * @param enabled true, um das Schreiben in Logdateien zu aktivieren; false, um es zu deaktivieren.
 */
void LLog::setFileLogging(bool enabled) {
	m_fileLogging = enabled;
	Preferences preferences;
	if (preferences.begin("debug", false)) {
		preferences.putBool("fileLogging", enabled);
		preferences.end();
	}
}

/**
 * @brief Gibt zurück, ob Dateilogging derzeit aktiviert ist.
 *
 * Der Wert wird bei Aufruf aus den Preferences geladen.
 *
 * @return true, wenn Dateilogging aktiv ist; sonst false.
 */
bool LLog::isFileLogging() {
	Preferences pref;
	if (pref.begin("debug", true)) {
		m_fileLogging = preferences.getBool("fileLogging", m_fileLogging);
		preferences.end();
	}
	return m_fileLogging;
}
