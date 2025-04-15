/**
 * @file WebServerManager.cpp
 * @brief Modul zur Verwaltung des Async-Webservers und WebSocket-Servers.
 *
 * Dieses Modul initialisiert und verwaltet einen Async-Webserver, der eine Single-Page-Applikation (SPA)
 * über LittleFS bereitstellt. Zusätzlich wird ein WebSocket-Server für Echtzeit-Kommunikation integriert.
 * JSON-basierte WebSocket-Nachrichten werden an registrierte Event-Handler weitergeleitet.
 *
 * Unterstützt außerdem REST-Endpunkte für den Zugriff auf Logdateien sowie regelmäßige Aufräumarbeiten
 * bei inaktiven WebSocket-Clients.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#include "WebServerManager.h"

#include <LittleFS.h>

/**
 * @brief Konstruktor für den WebServerManager.
 *
 * Initialisiert den Webserver und WebSocket auf dem angegebenen Port.
 *
 * @param port Der Port, auf dem der Webserver gestartet wird.
 */
WebServerManager::WebServerManager(uint16_t port) : server(port), ws("/ws") {
}

/**
 * @brief Initialisiert den Webserver, WebSocket und registriert die HTTP-Endpunkte.
 *
 * Diese Methode mountet das Dateisystem, registriert statische Inhalte, definiert REST-Endpunkte
 * und startet den Webserver. Auch der WebSocket-Handler wird initialisiert.
 */
void WebServerManager::init() {
	if (!LittleFS.begin()) {
		logger.error("[HTTP] LittleFS konnte nicht gemountet werden!");
		return;
	}
	logger.info("[HTTP] LittleFS erfolgreich gemountet.");

	// WebSocket-Event-Handler
	ws.onEvent(onEvent);
	server.addHandler(&ws);

	// SPA-Serving mit Fallback
	server.serveStatic("/", LittleFS, "/www/html").setDefaultFile("index.html");

	// REST-Endpunkt: einzelne Logdatei abrufen
	server.on("/logfile", HTTP_GET, [this](AsyncWebServerRequest *request) {
		logger.info("[HTTP] HTTP-Anfrage erhalten: /logfile");
		request->send(LittleFS, "/log.txt", "text/plain");
	});

	// REST-Endpunkt: Alle Logdateien auflisten
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

	// Fallback für SPA (Client-Side Routing)
	server.onNotFound([](AsyncWebServerRequest *request) { request->send(LittleFS, "/www/html/index.html", "text/html"); });

	server.begin();
	logger.info("[HTTP] Webserver und WebSocket-Server gestartet.");
	logger.info("[HTTP] WebSocket erreichbar unter: ws://" + WiFi.softAPIP().toString() + "/ws");
}

/**
 * @brief Entfernt inaktive WebSocket-Clients aus dem Speicher.
 *
 * Diese Methode sollte regelmäßig (z. B. im loop oder Task) aufgerufen werden,
 * um nicht mehr aktive Clients zu entfernen.
 */
void WebServerManager::cleanupWebSocketClients() {
	ws.cleanupClients();
}

/**
 * @brief Sendet eine Logdatei über HTTP zurück.
 *
 * Erwartet einen GET-Parameter `file`, welcher den Namen der Logdatei angibt.
 *
 * @param request Zeiger auf das eingehende Webserver-Request-Objekt.
 */
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
