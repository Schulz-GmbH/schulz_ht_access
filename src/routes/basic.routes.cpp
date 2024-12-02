
#include "../include/routes/basic.routes.h"

void initBasicRoutes(AsyncWebServer &server) {
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (LittleFS.exists("/index.html")) {
			request->send(LittleFS, "/index.html", "text/html");
		} else {
			request->send(404, "text/plain", "index.html nicht gefunden!");
		}
	});

	// // Route: Statische Dateien (z. B. Assets)
	server.on("/assets/*", HTTP_GET, [](AsyncWebServerRequest *request) {
		String path = request->url();  // Holt den Pfad aus der Anfrage, z.B. /assets/index.js
		if (LittleFS.exists(path)) {
			String contentType = "text/plain";
			if (path.endsWith(".js")) {
				contentType = "application/javascript";
			} else if (path.endsWith(".css")) {
				contentType = "text/css";
			} else if (path.endsWith(".svg")) {
				contentType = "image/svg+xml";
			} else if (path.endsWith(".woff")) {
				contentType = "font/woff";
			} else if (path.endsWith(".woff2")) {
				contentType = "font/woff2";
			} else if (path.endsWith(".ttf")) {
				contentType = "font/ttf";
			} else if (path.endsWith(".eot")) {
				contentType = "application/vnd.ms-fontobject";
			}

			request->send(LittleFS, path, contentType);
		} else {
			request->send(404, "text/plain", "Datei nicht gefunden");
		}
	});

	server.on("/css/*", HTTP_GET, [](AsyncWebServerRequest *request) {
		String path = request->url();
		if (LittleFS.exists(path)) {
			String contentType = "text/plain";
			if (path.endsWith(".css")) {
				contentType = "text/css";
			}
			request->send(LittleFS, path, contentType);
		} else {
			request->send(404, "text/plain", "Datei nicht gefunden");
		}
	});

	server.on("/js/*", HTTP_GET, [](AsyncWebServerRequest *request) {
		String path = request->url();
		if (LittleFS.exists(path)) {
			String contentType = "text/plain";
			if (path.endsWith(".js")) {
				contentType = "application/javascript";
			}
			request->send(LittleFS, path, contentType);
		} else {
			request->send(404, "text/plain", "Datei nicht gefunden");
		}
	});
}
