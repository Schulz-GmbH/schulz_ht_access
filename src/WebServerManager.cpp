/**
 * @file WebServerManager.cpp
 * @brief Modul zur Verwaltung des Async-Webservers und WebSocket-Servers.
 *
 * Dieses Modul initialisiert und verwaltet einen Async-Webserver, der eine Single-Page-Applikation (SPA)
 * von einer SD-Karte bereitstellt. Zudem wird ein WebSocket-Server integriert, der Echtzeit-Kommunikation
 * ermöglicht. Eingehende WebSocket-Nachrichten werden geparst und an entsprechende Event-Handler weitergeleitet,
 * die basierend auf dem Ereignistyp (System, Log, Serial) weitere Aktionen ausführen können.
 *
 * Die WebSocket-Kommunikation nutzt JSON-formatierte Nachrichten, wobei Antworten an den Client ebenfalls
 * JSON-basiert zurückgesendet werden.
 *
 * Zusätzlich bietet das Modul eine Methode zur regelmäßigen Bereinigung inaktiver WebSocket-Clients.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#include "WebServerManager.h"

#include <LittleFS.h>

/**
 * @brief Konstruktor für WebServerManager.
 *
 * Initialisiert Webserver und WebSocket-Server auf einem angegebenen Port.
 *
 * @param port Der Port, auf dem der Webserver läuft.
 */
WebServerManager::WebServerManager(uint16_t port) : server(port), ws("/ws") {
}

void WebServerManager::init() {
	// FFat mounten
	if (!LittleFS.begin()) {
		logger.error("[HTTP] LittleFS konnte nicht gemountet werden!");
		return;
	}
	logger.info("[HTTP] LittleFS erfolgreich gemountet.");
	// WebSocket-Event-Handler registrieren
	ws.onEvent(onEvent);
	server.addHandler(&ws);

	// Statische Dateien bereitstellen (SPA)
	server.serveStatic("/", LittleFS, "/www/html").setDefaultFile("index.html");

	// REST-Endpunkt Beispiel: Logdateien abrufen
	server.on("/logfile", HTTP_GET, [this](AsyncWebServerRequest *request) {
		logger.info("[HTTP] HTTP-Anfrage erhalten: /logfile");
		// serveLogFile(request);
		request->send(LittleFS, "/log.txt", "text/plain");
	});

	server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
		String output = "<ul>";
		File root = LittleFS.open("/logs");
		if (!root || !root.isDirectory()) {
			request->send(500, "text/html", "Log-Verzeichnis nicht gefunden!");
			return;
		}

		File file = root.openNextFile();
		while (file) {
			output += "<li>" + String(file.name()) + " (" + String(file.size()) + " Bytes)</li>";
			file = root.openNextFile();
		}
		output += "</ul>";

		request->send(200, "text/html", output);
	});

	// Fallback (SPA Routing-Unterstützung)
	server.onNotFound([](AsyncWebServerRequest *request) { request->send(LittleFS, "/www/html/index.html", "text/html"); });

	// Webserver starten
	server.begin();
	logger.info("[HTTP] Webserver und WebSocket-Server gestartet.");
	// Beispiel: Adresse des WebSocket-Servers ausgeben
	logger.info("[HTTP] WebSocket erreichbar unter: ws://" + WiFi.softAPIP().toString() + "/ws");
}

/**
 * @brief Bereinigt regelmäßig inaktive WebSocket-Clients.
 */
void WebServerManager::cleanupWebSocketClients() {
	ws.cleanupClients();
}

void WebServerManager::serveLogFile(AsyncWebServerRequest *request) {
	if (!request->hasParam("file")) {
		request->send(400, "text/plain", "Missing file parameter");
		return;
	}

	String fileName = "/logs/" + request->getParam("file")->value();

	if (!SD.exists(fileName)) {
		logger.error("[HTTP] " + request->getParam("file")->value() + " nicht gefunden!");
		request->send(404, "text/plain", request->getParam("file")->value() + " nicht gefunden!");
		return;
	}

	File file = SD.open(fileName, FILE_READ);
	if (!file) {
		logger.error("[HTTP] " + request->getParam("file")->value() + " konnte nicht geöffnet werden!");
		request->send(500, "text/plain", "Fehler beim Öffnen des Logs " + request->getParam("file")->value() + "!");
		return;
	}

	AsyncWebServerResponse *response = request->beginResponse(SD, fileName, "text/plain");
	response->addHeader("Access-Control-Allow-Origin", "*");
	response->addHeader("Access-Control-Allow-Methods", "GET, POST");
	response->addHeader("Access-Control-Allow-Headers", "Content-Type");
	request->send(response);
}
