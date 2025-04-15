/**
 * @file WebServerManager.h
 * @brief Header-Datei für das Webserver- und WebSocket-Handling.
 *
 * Deklariert die Klasse WebServerManager, welche einen AsyncWebServer
 * inklusive WebSocket-Funktionalität bereitstellt.
 *
 * Statische Dateien werden über LittleFS ausgeliefert. REST-Endpunkte
 * ermöglichen den Zugriff auf Logdateien. Die Klasse unterstützt auch
 * regelmäßige Aufräumroutinen für WebSocket-Clients.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <ESPAsyncWebServer.h>
#include <SD.h>

#include "LLog.h"
#include "WsEvents.h"
#include "global.h"

/**
 * @brief WebServerManager zur Verwaltung von HTTP- und WebSocket-Kommunikation.
 */
class WebServerManager {
   public:
	/**
	 * @brief Konstruktor.
	 *
	 * @param port Port, auf dem der Webserver starten soll (Standard: 80).
	 */
	WebServerManager(uint16_t port = 80);

	/**
	 * @brief Initialisiert den Webserver, WebSocket und REST-Endpunkte.
	 */
	void init();

	/**
	 * @brief Bereinigt regelmäßig inaktive WebSocket-Clients.
	 */
	void cleanupWebSocketClients();

	/**
	 * @brief Beantwortet Anfragen zum Abruf von Logdateien.
	 *
	 * Diese Methode erwartet einen `file`-Parameter, der die Logdatei benennt.
	 *
	 * @param request Zeiger auf die Anfrage.
	 */
	void serveLogFile(AsyncWebServerRequest *request);

   private:
	AsyncWebServer server;  ///< Instanz des Webservers
	AsyncWebSocket ws;      ///< Instanz des WebSocket-Servers
};

#endif  // WEBSERVER_MANAGER_H
