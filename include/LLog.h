#ifndef LLOG_H
#define LLOG_H

#include <Arduino.h>
#include <SD.h>
#include <esp_system.h>

#include <ctime>

class LLog {
   public:
	static LLog &getInstance();

	void print(const String &message);
	void println(const String &message);
	void debug(const String &message);
	void info(const String &message);
	void system(const String &message);
	void warn(const String &message);
	void error(const String &message);

	// Getter/Setter für den aktiven Modus
	static void setActive(bool state);
	static bool isActive();

   private:
	LLog() {
	}
	LLog(const LLog &) = delete;
	void operator=(const LLog &) = delete;

	void logMessage(const String &message, bool newLine);
	String getTimestamp();

	// Statische Variable
	static bool m_active;

	static File m_logFile;        // Die geöffnete Log-Datei
	static String m_logFileName;  // Enthält den Namen der aktuellen Log-Datei
};

extern LLog &logger;

#endif
