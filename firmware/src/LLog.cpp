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

#include <Arduino.h>
#include <LittleFS.h>  // Statt SD verwenden wir jetzt LittleFS

LLog &logger = LLog::getInstance();  ///< Globale Logger-Instanz

bool LLog::m_active = false;
File LLog::m_logFile;
String LLog::m_logFileName = "";

/**
 * @brief Liefert die Singleton-Instanz des Loggers.
 * @return Referenz auf die LLog-Instanz.
 */
LLog &LLog::getInstance() {
	static LLog instance;
	return instance;
}

/**
 * @brief Aktiviert oder deaktiviert das Logging.
 *
 * Beim Aktivieren wird ggf. eine neue Logdatei erzeugt und geöffnet.
 * Beim Deaktivieren wird die Datei sauber geschlossen.
 *
 * @param state true = Logging aktivieren, false = deaktivieren.
 */
void LLog::setActive(bool state) {
	if (state && !m_active) {
		// Aktuelle Zeit abfragen
		time_t now = time(nullptr);
		struct tm timeinfo;
		localtime_r(&now, &timeinfo);

		char filename[64];
		if (timeinfo.tm_year + 1900 == 1970) {
			uint32_t randomID = esp_random();
			snprintf(filename, sizeof(filename), "/logs/SystemLog-1970-01-01-%u.log", (unsigned)randomID);
			snprintf(filename, sizeof(filename), "/logs/log_1970-01-01-%u.log", (unsigned)randomID);
		} else {
			snprintf(filename, sizeof(filename), "/logs/log_%04d-%02d-%02d.log", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
		}

		m_logFileName = String(filename);
		// Datei explizit anlegen, wenn sie noch nicht existiert
		if (!LittleFS.exists(m_logFileName)) {
			File tmp = LittleFS.open(m_logFileName, "w");
			if (tmp) {
				tmp.close();
			}
		}

		m_logFile = LittleFS.open(m_logFileName, FILE_APPEND);

		if (!m_logFile) {
			Serial.println("[LLog] Fehler beim Öffnen der Logdatei!");
		} else {
			Serial.print("[LLog] Schreibe in Logdatei: ");
			Serial.println(m_logFileName);
		}
	} else if (!state && m_active) {
		if (m_logFile) {
			m_logFile.close();
		}
	}

	m_active = state;
}

/**
 * @brief Prüft, ob Logging aktiv ist.
 * @return true, wenn Logging aktiv ist, sonst false.
 */
bool LLog::isActive() {
	return m_active;
}

/**
 * @brief Erstellt einen formatierten Zeitstempel.
 * @return String im Format "YYYY-MM-DD hh:mm:ss"
 */
String LLog::getTimestamp() {
	time_t now = time(nullptr);
	struct tm timeinfo;
	localtime_r(&now, &timeinfo);

	char buffer[20];
	snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
	         timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
	return String(buffer);
}

/**
 * @brief Interne Logik zum Loggen einer Nachricht.
 * @param message Der zu loggende Text.
 * @param newLine true = Zeilenumbruch hinzufügen.
 * @param timestamp true = Zeitstempel voranstellen.
 */
void LLog::logMessage(const String &message, bool newLine, bool timestamp) {
	if (m_active) {
		String logEntry;
		if (timestamp) {
			// Zeitstempel hinzufügen
			logEntry = "[" + getTimestamp() + "] " + message;
		} else {
			logEntry = " " + message;
		}

		// Seriell ausgeben
		if (newLine) {
			Serial.println(logEntry);
		} else {
			Serial.print(logEntry);
		}

		// Gleichzeitig in die geöffnete Datei schreiben
		if (m_logFile) {
			if (newLine) {
				m_logFile.println(logEntry);
			} else {
				m_logFile.print(logEntry);
			}
			// Optional: sofort flushen
			m_logFile.flush();
		}
	}
}

/// @copydoc LLog::debug
void LLog::debug(const String &message, bool newLine) {
	logMessage("[DEBUG]" + message, newLine, true);
}

/// @copydoc LLog::info
void LLog::info(const String &message, bool newLine) {
	logMessage("[INFO] " + message, newLine, true);
}

/// @copydoc LLog::system
void LLog::system(const String &message, bool newLine) {
	logMessage("[SYSTEM] " + message, newLine, true);
}

/// @copydoc LLog::warn
void LLog::warn(const String &message, bool newLine) {
	logMessage("[WARNING] " + message, newLine, true);
}

/// @copydoc LLog::error
void LLog::error(const String &message, bool newLine) {
	logMessage("[ERROR] " + message, newLine, true);
}

/// @copydoc LLog::print
void LLog::print(const String &message, bool timestamp) {
	logMessage(message, false, timestamp);
}

/// @copydoc LLog::println
void LLog::println(const String &message, bool timestamp) {
	logMessage(message, true, timestamp);
}
