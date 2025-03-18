#include "LLog.h"

// Globale Singleton-Instanz
LLog &logger = LLog::getInstance();

// Statische Variablen initialisieren
bool LLog::m_active = false;  // Default (kann aber auch true sein)

// Singleton-Getter
LLog &LLog::getInstance() {
	static LLog instance;
	return instance;
}

// Setter/Getter für den aktiven Modus
void LLog::setActive(bool state) {
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

		if (newLine) {
			Serial.println(logEntry);
		} else {
			Serial.print(logEntry);
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
	logMessage("[WRNING] " + message, true);
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
