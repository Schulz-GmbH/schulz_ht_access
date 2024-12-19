
#include "../include/routes/basic.routes.h"

#include <SD.h>
#include <SPI.h>

extern SemaphoreHandle_t sdMutex;  // Externe Deklaration des Mutex

void initBasicRoutes(AsyncWebServer &server) {
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (xSemaphoreTake(sdMutex, portMAX_DELAY) == pdTRUE) {
			String path = request->url();  // Pfad aus Anfrage holen, z.B. /css/style.css
			Serial.println("Angeforderter Pfad: " + path);

			if (SD.exists(path)) {
				File file = SD.open("/index.html", FILE_READ);
				if (file) {
					// String contentType = "text/css";
					request->send(file, path, getContentType("/index.html"));
					Serial.println("JavaScript-Datei ausgeliefert: " + path);
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
	server.on("/assets/*", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (xSemaphoreTake(sdMutex, portMAX_DELAY) == pdTRUE) {
			String path = request->url();  // Holt den Pfad aus der Anfrage, z.B. /assets/index.js
			Serial.println("Angeforderter Pfad: " + path);
			if (SD.exists(path)) {
				File file = SD.open(path, FILE_READ);
				if (file) {
					request->send(file, getContentType(path));  // Datei senden ohne manuelles Schließen
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
	});

	server.on("/js/*", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (xSemaphoreTake(sdMutex, portMAX_DELAY) == pdTRUE) {
			String path = request->url();  // Pfad aus Anfrage holen, z.B. /js/index.js
			Serial.println("Angeforderter Pfad: " + path);

			if (SD.exists(path)) {
				File file = SD.open(path, FILE_READ);
				if (file) {
					String contentType = "application/javascript";
					request->send(file, path, getContentType(path));
					Serial.println("JavaScript-Datei ausgeliefert: " + path);
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

	server.on("/css/*", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (xSemaphoreTake(sdMutex, portMAX_DELAY) == pdTRUE) {
			String path = request->url();  // Pfad aus Anfrage holen, z.B. /css/style.css
			Serial.println("Angeforderter Pfad: " + path);

			if (SD.exists(path)) {
				File file = SD.open(path, FILE_READ);
				if (file) {
					String contentType = "text/css";
					request->send(file, path, getContentType(path));
					Serial.println("JavaScript-Datei ausgeliefert: " + path);
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
