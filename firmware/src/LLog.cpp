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
 * Verzeichnisse zur Speicherung der Log-Dateien. Räum beim Intitialiseren automatisch alte Logdateien auf.
 * Liest den Status des File-Loggings aus den Preferences.
 * Wenn LittleFS nicht gemountet werden kann, wird eine Fehlermeldung ausgegeben.
 * Wenn das Verzeichnis /logs nicht existiert, wird es erstellt.
 * Wenn das Verzeichnis /logs/system nicht existiert, wird es erstellt.
 * Wenn das Verzeichnis /logs/device nicht existiert, wird es erstellt.
 */
LLog::LLog() {
	if (!LittleFS.begin()) {
		logger.log({"system", "error", "filesystem"}, "LittleFS konnte nicht gemountet werden!");
	}
	if (!LittleFS.exists("/logs")) LittleFS.mkdir("/logs");
	if (!LittleFS.exists("/logs/system")) LittleFS.mkdir("/logs/system");
	// File-Logging-Status aus Preferences laden
	Preferences pref;
	if (pref.begin("debug", true)) {
		m_fileLogging = pref.getBool("fileLogging", false);
		pref.end();
	}
	// Automatisches Aufräumen beim Start
	clearLargeLogs();
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

/**
 * @brief Gibt eine Log-Nachricht mit mehreren Events aus.
 *
 * Die Nachricht wird auf der seriellen Schnittstelle und – je nach Konfiguration – auch
 * in mehrere Logdateien geschrieben (eine pro Event-Kategorie).
 *
 * @param levels Liste von Event-Kategorien wie "info", "system", "socket".
 * @param message Die zu loggende Nachricht.
 * @param newLine true, um Zeilenumbruch anzuhängen.
 * @param timestamp true, um Zeitstempel voranzustellen.
 */
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
		logger.log({"system", "error", "filsystem", "llog"}, "Fehler beim Öffnen von " + path);
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
 * @brief Leert die Log-Datei für ein bestimmtes Event.
 *
 * Öffnet die Datei im Schreibmodus, wodurch ihr Inhalt überschrieben wird.
 *
 * @param event Name des Events (z. B. "info", "debug").
 */
void LLog::clearLog(const String &event) {
	String path = "/logs/system/" + event + ".log";
	// Öffnen im WRITE-Modus leert die Datei
	File f = LittleFS.open(path, FILE_WRITE);
	if (f) f.close();
}

/**
 * @brief Leert mehrere Log-Dateien.
 *
 * Diese Methode ruft `clearLog()` für jedes übergebene Event auf.
 *
 * @param events Liste von Event-Namen, deren Logs geleert werden sollen.
 */
void LLog::clearLogs(const std::vector<String> &events) {
	for (const auto &evt : events) {
		clearLog(evt);
	}
}

/**
 * @brief Löscht Logdateien, deren Größe das Limit überschreitet.
 *
 * Diese Methode prüft die Größe aller bekannten Event-Logdateien im Verzeichnis
 * `/logs/system/` und leert jene, die größer als `maxSize` sind.
 *
 * @param maxSize Maximale erlaubte Dateigröße in Bytes (Standard: 32.000).
 */
void LLog::clearLargeLogs(size_t maxSize) {
	for (const auto &evt : Events) {
		String fname = evt + ".log";
		String path = "/logs/system/" + fname;
		if (LittleFS.exists(path)) {
			File f = LittleFS.open(path, "r");
			if (f) {
				if (f.size() > maxSize) {
					f.close();
					// Leeren
					File t = LittleFS.open(path, FILE_WRITE);
					if (t) t.close();
				} else {
					f.close();
				}
			}
		}
	}
}

/**
 * @brief Erstellt oder überschreibt eine Log-Datei im Verzeichnis `/logs/device`.
 *
 * Diese Funktion kann genutzt werden, um gerätespezifische Logs zu erstellen,
 * z. B. für empfangene Nachrichten oder Konfigurationen.
 *
 * @param filename Dateiname der zu erstellenden Logdatei (ohne Pfad).
 * @param content Inhalt, der in die Datei geschrieben wird.
 */

void LLog::createDeviceLog(const String &filename, const String &content) {
	const char *dirPath = "/logs/device";
	if (!LittleFS.exists(dirPath)) {
		LittleFS.mkdir(dirPath);
	}
	String fullPath = String(dirPath) + "/" + filename;
	File f = LittleFS.open(fullPath, FILE_WRITE);
	if (!f) {
		logger.log({"system", "error", "filesystem", "llog"}, "Kann Datei nicht erstellen: " + fullPath);
		return;
	}
	f.print(content);
	f.close();
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
		String message = "File-Logging " + String(enabled ? "aktiviert" : "deaktiviert");
		LLog::getInstance().logMessage({"system", "llog"}, message, true, true);
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
