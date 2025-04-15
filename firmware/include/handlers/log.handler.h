#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// Log-Klasse als Singleton
class LLog {
   public:
	static LLog &getInstance();  // Singleton-Zugriff

	void print(const String &message);
	void println(const String &message);
	void debug(const String &message);
	void info(const String &message);
	void system(const String &message);
	void warn(const String &message);
	void error(const String &message);
	void http(const String &message);

	void start();
	void stop();

	static bool loggingActive;
	static bool debugLog;

   private:
	LLog() {
	}  // Verhindert direkte Instanzierung
	LLog(const LLog &) = delete;  // Kopieren nicht erlaubt
	void operator=(const LLog &) = delete;

	void logMessage(const String &message, bool newLine);
	String getTimestamp();

	String logFileName = "";
};

extern LLog &logger;  // Globale Instanz

void handleLogEvent(const String &setting, const String &value, AsyncWebSocketClient *client);
void listLogs(AsyncWebSocketClient *client);
void serveLogFile(AsyncWebServerRequest *request);

#endif
