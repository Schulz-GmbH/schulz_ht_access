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
	// WebSocket-Event-Handler registrieren
	ws.onEvent(onEvent);
	server.addHandler(&ws);

	// Statische Dateien bereitstellen (SPA)
	server.serveStatic("/", SD, "/www/html/").setDefaultFile("index.html");

	// REST-Endpunkt Beispiel: Logdateien abrufen
	server.on("/logfile", HTTP_GET, [](AsyncWebServerRequest *request) {
		logger.info("[HTTP] HTTP-Anfrage erhalten: /logfile");
		// serveLogFile(request); // ToDo: Implementierung
	});

	// Fallback (SPA Routing-Unterstützung)
	server.onNotFound([](AsyncWebServerRequest *request) { request->send(SD, "/www/html/index.html", "text/html"); });

	// Webserver starten
	server.begin();
	logger.info("Webserver und WebSocket-Server gestartet.");
	// Beispiel: Adresse des WebSocket-Servers ausgeben
	logger.info("WebSocket erreichbar unter: ws://" + WiFi.softAPIP().toString() + "/ws");
}

/**
 * @brief Bereinigt regelmäßig inaktive WebSocket-Clients.
 */
void WebServerManager::cleanupWebSocketClients() {
	ws.cleanupClients();
}
