#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <ESPAsyncWebServer.h>

/**
 * @file WebServerManager.h
 * @brief Header für den HTTP-Only Webserver, der statische Dateien und Log-Endpunkte bereitstellt.
 *
 * Diese Klasse verwaltet ausschließlich HTTP-Endpunkte für eine Weboberfläche und zur
 * Anzeige von Logdateien. WebSocket-Funktionalitäten werden hier nicht berücksichtigt.
 *
 * Die bereitgestellten Endpunkte beinhalten:
 * - Anzeige aller Systemlogdateien im HTML-Format
 * - Einzelne Logdateien (z. B. `info.log`) direkt im Browser anzeigen
 * - Gerätelogdateien (über `/logs/device`) abrufen
 * - SPA-Frontend ausliefern
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */
class WebServerManager {
   public:
	/**
	 * @brief Registriert alle relevanten HTTP-Routen am übergebenen Server.
	 *
	 * Dazu gehören:
	 * - `/logs`: HTML-Liste aller Systemlogdateien
	 * - `/logfile?level=...`: Einzelne Systemlogdatei
	 * - `/logs/device?file=...`: Gerätespezifische Logdatei
	 * - statische Ressourcen unter `/www/html/`
	 * - Fallback-Routing für SPA
	 *
	 * @param server Referenz auf die `AsyncWebServer`-Instanz.
	 */
	void init(AsyncWebServer &server);

	/**
	 * @brief HTTP-Handler für GET /logs.
	 *
	 * Sendet eine HTML-Liste aller im Dateisystem gespeicherten System-Logdateien.
	 *
	 * @param request Eingehende HTTP-Anfrage.
	 */
	void serveSystemLogList(AsyncWebServerRequest *request);

	/**
	 * @brief HTTP-Handler für GET /logfile?level=…
	 *
	 * Sendet den Inhalt einer bestimmten Systemlogdatei im HTML-Format mit Hervorhebungen.
	 *
	 * @param request Eingehende HTTP-Anfrage.
	 */
	void serveSystemLog(AsyncWebServerRequest *request);

	/**
	 * @brief HTTP-Handler für GET /logs/device?file=…
	 *
	 * Sendet den Inhalt einer Gerätelogdatei als `text/plain`.
	 *
	 * @param request Eingehende HTTP-Anfrage.
	 */
	void serveDeviceLog(AsyncWebServerRequest *request);

   private:
	/**
	 * @brief Erzeugt HTML-Code zur Anzeige der Systemlogdateien.
	 *
	 * Diese Methode wird intern von `serveSystemLogList` verwendet.
	 *
	 * @return HTML-Seite als String.
	 */
	String buildSystemLogListHtml();
};

#endif  // WEBSERVERMANAGER_H
