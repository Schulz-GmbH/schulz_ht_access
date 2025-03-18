#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <ESPAsyncWebServer.h>
#include <SD.h>

#include "LLog.h"
#include "WsEvents.h"
#include "global.h"

class WebServerManager {
   public:
	WebServerManager(uint16_t port = 80);

	/// Initialisiert den Webserver, registriert den WebSocket und REST-Endpunkte
	void init();

	/// Räumt die WebSocket-Clients auf (soll im loop() regelmäßig aufgerufen werden)
	void cleanupWebSocketClients();

   private:
	AsyncWebServer server;
	AsyncWebSocket ws;
};

#endif  // WEBSERVER_MANAGER_H
