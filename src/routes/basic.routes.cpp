
#include "../include/routes/basic.routes.h"

#include <SD.h>
#include <SPI.h>

extern SemaphoreHandle_t sdMutex;      // Externe Deklaration des Mutex
const String BASE_PATH = "/www/html";  // Basis-Pfad auf der SD-Karte

void initBasicRoutes(AsyncWebServer &server) {
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (xSemaphoreTake(sdMutex, portMAX_DELAY) == pdTRUE) {
			// String path = request->url();  // Pfad aus Anfrage holen, z.B. /css/style.css
			Serial.println("Angeforderter Pfad: " + request->url());
			String path = BASE_PATH + "/index.html";  // Basis-Pfad verwenden
			Serial.println("Angeforderter Pfad: " + path);

			if (SD.exists(path)) {
				File file = SD.open(path, FILE_READ);
				if (file) {
					request->send(file, path, getContentType(path));
					Serial.println("HTML-Datei ausgeliefert: " + path);
				} else {
					Serial.println("Fehler beim Öffnen der Datei: " + path);
					request->send(404, "text/plain", "Datei konnte nicht geöffnet werden");
				}
			} else {
				Serial.println("Datei nicht gefunden auf der SD-Karte: " + path);
				request->send(404, "text/plain", "Datei nicht gefunden");
			}
			xSemaphoreGive(sdMutex);  // Gebe den Mutex wieder frei
		} else {
			request->send(500, "text/plain", "Fehler beim Zugriff auf SD-Karte");
		}
	});

	// Route: Statische Dateien für Assets, CSS und JavaScript von der SD-Karte
	server.on("/assets/*", HTTP_GET, [](AsyncWebServerRequest *request) { handleFileRequest(request, "/assets"); });

	server.on("/js/*", HTTP_GET, [](AsyncWebServerRequest *request) { handleFileRequest(request, "/js"); });

	server.on("/css/*", HTTP_GET, [](AsyncWebServerRequest *request) { handleFileRequest(request, "/css"); });
}

void handleFileRequest(AsyncWebServerRequest *request, const String &subPath) {
	if (xSemaphoreTake(sdMutex, portMAX_DELAY) == pdTRUE) {
		String path = BASE_PATH + request->url();  // Basis-Pfad mit Anfrage kombinieren
		Serial.println("Angeforderter Pfad: " + path);

		if (SD.exists(path)) {
			File file = SD.open(path, FILE_READ);
			if (file) {
				request->send(file, path, getContentType(path));
				Serial.println(path + " ausgeliefert.");
			} else {
				Serial.println("Fehler beim Öffnen der Datei: " + path);
				request->send(404, "text/plain", "Datei konnte nicht geöffnet werden");
			}
		} else {
			Serial.println("Datei nicht gefunden auf der SD-Karte: " + path);
			request->send(404, "text/plain", "Datei nicht gefunden");
		}
		xSemaphoreGive(sdMutex);  // Gebe den Mutex wieder frei
	} else {
		request->send(500, "text/plain", "Fehler beim Zugriff auf SD-Karte");
	}
}

String getContentType(String path) {
	if (path.endsWith(".html")) {
		return "text/html";
	} else if (path.endsWith(".js")) {
		return "application/javascript";
	} else if (path.endsWith(".css")) {
		return "text/css";
	} else if (path.endsWith(".svg")) {
		return "image/svg+xml";
	} else if (path.endsWith(".woff")) {
		return "font/woff";
	} else if (path.endsWith(".woff2")) {
		return "font/woff2";
	} else if (path.endsWith(".ttf")) {
		return "font/ttf";
	} else if (path.endsWith(".eot")) {
		return "application/vnd.ms-fontobject";
	}
	return "text/plain";  // Standardwert, falls keine Zuordnung gefunden wurde
}
