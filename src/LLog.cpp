#include "LLog.h"

// Globale Singleton-Instanz
LLog &logger = LLog::getInstance();

// Statische Variablen initialisieren
bool LLog::m_active = false;  // Default (kann aber auch true sein)
File LLog::m_logFile;
String LLog::m_logFileName = "";

// Singleton-Getter
LLog &LLog::getInstance() {
	static LLog instance;
	return instance;
}

// Setter/Getter für den aktiven Modus
void LLog::setActive(bool state) {
	// Wenn wir den Logger einschalten
	if (state && !m_active) {
		// Aktuelle Zeit abfragen
		time_t now = time(nullptr);
		struct tm timeinfo;
		localtime_r(&now, &timeinfo);

		// Dateiname vorbereiten
		char filename[64];

		// Prüfen, ob die Uhrzeit auf 1970 steht -> RTC/Internetzeit nicht vorhanden
		if (timeinfo.tm_year + 1900 == 1970) {
			// => stattdessen eine eindeutige ID anhängen (z. B. esp_random())
			uint32_t randomID = esp_random();
			snprintf(filename, sizeof(filename),
			         "/logs/SystemLog-1970-01-01-%u.log",  // z. B. SystemLog-1970-01-01-123456789.log
			         (unsigned)randomID);
		} else {
			// Uhrzeit plausibel, also normalen Dateinamen mit Datum
			snprintf(filename, sizeof(filename), "/logs/SystemLog-%04d-%02d-%02d.log", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,
			         timeinfo.tm_mday);
		}

		m_logFileName = filename;
		m_logFile = SD.open(m_logFileName, FILE_APPEND);

		if (!m_logFile) {
			Serial.println("[LLog] Fehler beim Öffnen der Logdatei!");
		} else {
			Serial.print("[LLog] Schreibe in Logdatei: ");
			Serial.println(m_logFileName);
		}
	}
	// Wenn wir den Logger ausschalten und eine Datei offen ist
	else if (!state && m_active) {
		if (m_logFile) {
			m_logFile.close();
		}
	}

	m_active = state;
}

bool LLog::isActive() {
	return m_active;
}

// Zeitstempel generieren
String LLog::getTimestamp() {
	time_t now = time(nullptr);
	struct tm timeinfo;
	localtime_r(&now, &timeinfo);

	char buffer[20];
	snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
	         timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
	return String(buffer);
}

// Generelle Log-Funktion ohne Level, aber mit Zeitstempel und optionalem Zeilenumbruch
void LLog::logMessage(const String &message, bool newLine) {
	if (m_active) {
		String logEntry = "[" + getTimestamp() + "] " + message;

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

// Logging-Methoden für verschiedene Log-Level
void LLog::debug(const String &message) {
	logMessage("[DEBUG] " + message, true);
}
void LLog::info(const String &message) {
	logMessage("[INFO] " + message, true);
}
void LLog::system(const String &message) {
	logMessage("[SYSTEM] " + message, true);
}
void LLog::warn(const String &message) {
	logMessage("[WARNING] " + message, true);
}
void LLog::error(const String &message) {
	logMessage("[ERROR] " + message, true);
}

// Neue Methoden für `print()` und `printLn()` (nutzen `logMessage()`)
void LLog::print(const String &message) {
	logMessage(message, false);  // Ohne Zeilenumbruch
}

void LLog::println(const String &message) {
	logMessage(message, true);  // Mit Zeilenumbruch
}
