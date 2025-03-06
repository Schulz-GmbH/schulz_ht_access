#include "handlers/log.handler.h"

#include <ArduinoJson.h>
#include <SD.h>

#include <ctime>

#include "handlers/helper.handler.h"
#include "handlers/preferences.handler.h"
#include "handlers/response.handler.h"

// Globale Singleton-Instanz
LLog &logger = LLog::getInstance();

// Statische Variablen initialisieren
bool LLog::loggingActive = false;
bool LLog::debugLog = true;

// Singleton-Getter
LLog &LLog::getInstance() {
	static LLog instance;
	return instance;
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
	String logEntry = "[" + getTimestamp() + "] " + message;

	// SD-Karten-Logging
	if (debugLog && loggingActive) {
		File logFile = SD.open(logFileName, FILE_APPEND);
		if (logFile) {
			if (newLine) {
				logFile.println(logEntry);
			} else {
				logFile.print(logEntry);
			}
			logFile.flush();
			logFile.close();
		}
	}

	// Serielle Ausgabe
	if (newLine) {
		Serial.println(logEntry);
	} else {
		Serial.print(logEntry);
	}
}

// Logging starten
void LLog::start() {
	if (!SD.exists("/logs")) {
		SD.mkdir("/logs");
	}

	logFileName = "/logs/log.txt";
	File logFile = SD.open(logFileName, FILE_WRITE);

	if (logFile) {
		info("Logging gestartet");
		loggingActive = true;
		logFile.close();
	} else {
		error("Fehler beim Öffnen der Log-Datei!");
	}
}

// Logging stoppen
void LLog::stop() {
	if (loggingActive) {
		loggingActive = false;
		info("Logging gestoppt");
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
void LLog::http(const String &message) {
	logMessage("[HTTP] " + message, true);
}

// Neue Methoden für `print()` und `printLn()` (nutzen `logMessage()`)
void LLog::print(const String &message) {
	logMessage(message, false);  // Ohne Zeilenumbruch
}

void LLog::println(const String &message) {
	logMessage(message, true);  // Mit Zeilenumbruch
}

// Log-Events verarbeiten
void handleLogEvent(const String &setting, const String &value, AsyncWebSocketClient *client) {
	preferences.begin("system", false);
	if (setting == "debug") {
		if (value == "set:on") {
			preferences.putBool("debug", true);
		Log:
			logger.info("Erweitertes Logging aktiviert");
			LLog::debugLog = true;
			sendResponse(client, "log", "debug", "on", "Erweitertes Logging aktiviert");
		} else if (value == "set:off") {
			preferences.putBool("debug", false);
			LLog::debugLog = false;
			logger.info("Erweitertes Logging deaktiviert");
			sendResponse(client, "log", "debug", "off", "Erweitertes Logging deaktiviert");
		} else if (value == "status") {
			bool debugState = preferences.getBool("debug", false);
			String state = debugState ? "on" : "off";
			logger.info("Erweitertes Logging ist " + state);
			sendResponse(client, "log", "debug", state, state == "on" ? "Erweitertes Logging aktiviert" : "Erweitertes Logging deaktiviert");
			// sendResponse(client, "log", "status", "status", state);
		}
	} else if (setting == "list") {
		listLogs(client);
	} else if (setting == "rename") {
		// TODO: rename logfile
	} else if (setting == "delete") {
		// TODO: delete logfile
	} else {
		logger.error("Log: Unbekannter Log-Befehl");
		client->text("{\"event\":\"error\",\"details\":\"Unknown log command\"}");
	}
	preferences.end();
}

// Liste der Logs abrufen
void listLogs(AsyncWebSocketClient *client) {
	StaticJsonDocument<1024> jsonResponse;
	jsonResponse["event"] = "log";
	jsonResponse["action"] = "list";
	JsonArray logs = jsonResponse.createNestedArray("logs");

	File dir = SD.open("/logs");
	if (!dir || !dir.isDirectory()) {
		logger.error("/logs Verzeichnis nicht gefunden.");
		return;
	}

	File file = dir.openNextFile();
	while (file) {
		JsonObject logEntry = logs.createNestedObject();
		logEntry["name"] = String(file.name());
		logEntry["size"] = file.size();
		logEntry["date"] = (int)file.getLastWrite();
		file = dir.openNextFile();
	}
	dir.close();

	String response;
	serializeJson(jsonResponse, response);
	response.replace("\r", "");
	response.replace("\n", "");

	logger.info("Log-Liste gesendet");
	sendResponse(client, "log", "list", "success", logs, "");
}

// Log-Datei über HTTP bereitstellen
void serveLogFile(AsyncWebServerRequest *request) {
	if (!request->hasParam("file")) {
		request->send(400, "text/plain", "Missing file parameter");
		return;
	}

	String fileName = "/logs/" + request->getParam("file")->value();
	logger.info(request->getParam("file")->value() + " wird angefordert.");

	if (!SD.exists(fileName)) {
		logger.error(request->getParam("file")->value() + " nicht gefunden!");
		request->send(404, "text/plain", request->getParam("file")->value() + " nicht gefunden!");
		return;
	}

	File file = SD.open(fileName, FILE_READ);
	if (!file) {
		logger.error(request->getParam("file")->value() + " konnte nicht geöffnet werden!");
		request->send(500, "text/plain", "Fehler beim Öffnen des Logs " + request->getParam("file")->value() + "!");
		return;
	}
	logger.info(request->getParam("file")->value() + " wird geöffnet");

	AsyncWebServerResponse *response = request->beginResponse(SD, fileName, "text/plain");
	response->addHeader("Access-Control-Allow-Origin", "*");
	response->addHeader("Access-Control-Allow-Methods", "GET, POST");
	response->addHeader("Access-Control-Allow-Headers", "Content-Type");
	request->send(response);
}
