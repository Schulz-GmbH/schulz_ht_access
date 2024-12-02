#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <MDNS.h>

#include "handlers/wifi.handler.h"
#include "routes/routes.h"

// Netzwerk Konfiguration für AP
const char *apSSID = "ESP32-AccessPoint";
const char *apPassword = "12345678";

// Webserver auf Port 80 initialisieren
AsyncWebServer server(80);

void setup() {
	// Serielle Kommunikation starten
	Serial.begin(9600);

	// Statische IP-Konfiguration
	IPAddress local_IP(192, 168, 5, 1);  // IP-Adresse des AP
	IPAddress gateway(192, 168, 5, 1);   // Gateway-Adresse (gleich der AP-IP)
	IPAddress subnet(255, 255, 255, 0);  // Subnetzmaske

	// WiFi Access Point starten
	WiFi.softAPConfig(local_IP, gateway, subnet);
	WiFi.softAP(apSSID, apPassword);
	Serial.println("Access Point gestartet!");
	Serial.print("AP IP-Adresse: ");
	Serial.println(WiFi.softAPIP());

	// STA-Modus prüfen und verbinden
	String ssid, password;
	if (isWiFiEnabled && readWiFiConfig(ssid, password)) {
		connectToWiFi(ssid, password);
	} else {
		Serial.println("WLAN-Konfigurationsdatei fehlt oder ist leer. Keine Verbindung hergestellt.");
	}

	// mDNS Service starten
	if (!MDNS.begin("esp32")) {
		Serial.println("Fehler beim Starten von mDNS");
	} else {
		Serial.println("mDNS-Dienst gestartet, erreichbar unter esp32.local");
	}

	// Routen initialisieren
	initBasicRoutes(server);
	initWiFiRoutes(server);

	// Webserver starten
	server.begin();
	Serial.println("Webserver gestartet.");
}

void loop() {
	// put your main code here, to run repeatedly:
}
