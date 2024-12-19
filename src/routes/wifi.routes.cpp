
#include "../include/routes/wifi.routes.h"

#include <ArduinoJson.h>

#include "../include/handlers/cors.handler.h"
#include "../include/handlers/wifi.handler.h"

void initWiFiRoutes(AsyncWebServer &server) {
	server.on("/wifi/status", HTTP_GET, [](AsyncWebServerRequest *request) {
		String status = getWiFiStatus();
		request->send(jsonResponse(request, 200, status));
	});

	server.on("/wifi/connect", HTTP_GET, [](AsyncWebServerRequest *request) {
		String ssid, password;
		if (readWiFiConfig(ssid, password)) {
			connectToWiFi(ssid, password);
			request->send(jsonResponse(request, 200, "WLAN verbunden."));
		} else {
			request->send(jsonResponse(request, 500, "WLAN konnte nicht verbunden werden."));
		}
	});

	server.on(
	    "/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
	    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
		    // JSON-Daten parsen
		    // StaticJsonDocument<200> jsonDoc;
		    DynamicJsonDocument jsonDoc(200);
		    DeserializationError error = deserializeJson(jsonDoc, data, len);

		    if (error) {
			    Serial.println("Fehler beim Parsen der JSON-Daten: ");
			    Serial.println(error.c_str());
			    // request->send(400, "application/json", "{\"error\": \"Ungültiges JSON-Format.\"}");

			    AsyncWebServerResponse *response = request->beginResponse(400, "application/json", "{\"error\": \"Ungültiges Daten-Format.\"}");
			    addCORSHeaders(response);  // CORS-Header hinzufügen
			    request->send(response);
			    return;
		    }

		    // Extrahiere SSID und Passwort aus JSON
		    // if (jsonDoc.containsKey("ssid") && jsonDoc.containsKey("password")) {
		    //     String ssid = jsonDoc["ssid"].as<String>();
		    //     String password = jsonDoc["password"].as<String>();
		    if (jsonDoc["ssid"].is<const char *>() && jsonDoc["password"].is<const char *>()) {
			    const char *ssid = jsonDoc["ssid"];
			    const char *password = jsonDoc["password"];

			    // Serial.println("Empfangene Daten:");
			    // Serial.println("SSID: " + ssid);
			    // Serial.println("Password: " + password);

			    // Speichere die WLAN-Daten
			    if (saveWiFiConfig(ssid, password)) {
				    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\": \"WLAN-Daten gespeichert.\"}");
				    addCORSHeaders(response);  // CORS-Header hinzufügen
				    request->send(response);
			    } else {
				    AsyncWebServerResponse *response =
				        request->beginResponse(500, "application/json", "{\"error\": \"Fehler beim Speichern der WLAN-Daten.\"}");
				    addCORSHeaders(response);  // CORS-Header hinzufügen
				    request->send(response);
			    }
		    } else {
			    AsyncWebServerResponse *response = request->beginResponse(400, "application/json", "{\"error\": \"SSID oder Passwort fehlt.\"}");
			    addCORSHeaders(response);  // CORS-Header hinzufügen
			    request->send(response);
		    }
	    });

	// Route: WLAN-Daten abrufen
	server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
		String ssid, password;
		if (readWiFiConfig(ssid, password)) {
			String jsonResponse = "{\"ssid\": \"" + ssid + "\", \"password\": \"" + password + "\"}";
			AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonResponse);
			addCORSHeaders(response);  // CORS-Header hinzufügen
			request->send(response);
		} else {
			AsyncWebServerResponse *response = request->beginResponse(404, "application/json", "{\"error\": \"Keine WLAN-Daten gefunden.\"}");
			addCORSHeaders(response);  // CORS-Header hinzufügen
			request->send(response);
		}
	});
}
