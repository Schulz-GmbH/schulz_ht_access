/**
 * @file LLog.h
 * @brief Header-Datei für das Logging-Singleton-Modul LLog.
 *
 * Deklariert die Singleton-Klasse LLog zur einfachen, strukturierten Erzeugung von Logeinträgen.
 * Unterstützt Logging auf serielle Schnittstelle sowie persistentes Logging in Dateien auf einer SD-Karte.
 *
 * Unterstützte Log-Level: DEBUG, INFO, SYSTEM, WARNING, ERROR.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#ifndef LLOG_H
#define LLOG_H

#include <Arduino.h>
#include <SD.h>
#include <esp_system.h>

#include <ctime>

class LLog {
   public:
	static LLog &getInstance();

	void print(const String &message, bool timestamp = true);
	void println(const String &message, bool timestamp = true);
	void debug(const String &message, bool newLine = true);
	void info(const String &message, bool newLine = true);
	void system(const String &message, bool newLine = true);
	void warn(const String &message, bool newLine = true);
	void error(const String &message, bool newLine = true);

	// Getter/Setter für den aktiven Modus
	static void setActive(bool state);
	static bool isActive();

   private:
	LLog() {
	}
	LLog(const LLog &) = delete;
	void operator=(const LLog &) = delete;

	void logMessage(const String &message, bool newLine, bool timestamp = true);
	String getTimestamp();

	// Statische Variable
	static bool m_active;

	static File m_logFile;        // Die geöffnete Log-Datei
	static String m_logFileName;  // Enthält den Namen der aktuellen Log-Datei
};

extern LLog &logger;

#endif
