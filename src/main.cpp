#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <MDNS.h>

#include "handlers/card.handler.h"
#include "handlers/wifi.handler.h"
#include "routes/routes.h"

const int chipSelect = 5;  // SD-Karten-Pin

// Netzwerk Konfiguration für AP
const char *apSSID = "ESP32-AccessPoint";
const char *apPassword = "12345678";

// Webserver auf Port 80 initialisieren
AsyncWebServer server(80);

// Funktion, die den Webserver startet
void startWebServer() {
	// Routen initialisieren
	initErrorRoutes(server);
	initBasicRoutes(server);
	initWiFiRoutes(server);

	// Webserver starten
	server.begin();
	Serial.println("Webserver gestartet.");
}

// Task-Funktion für den Webserver
void webServerTask(void *parameter) {
	// WiFi Access Point starten
	IPAddress local_IP(192, 168, 5, 1);  // IP-Adresse des AP
	IPAddress gateway(192, 168, 5, 1);   // Gateway-Adresse (gleich der AP-IP)
	IPAddress subnet(255, 255, 255, 0);  // Subnetzmaske

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

	// Webserver starten
	startWebServer();

	// Unendliche Schleife, damit der Task nicht beendet wird
	for (;;) {
		delay(1000);
	}
}

void setup() {
	// Serielle Kommunikation starten
	Serial.begin(9600);

	Serial.println("Initialisiere externen Speicher...");
	delay(1000);  // Eine kurze Verzögerung vor dem Initialisieren kann helfen

	// SdSpiConfig erstellen, um die SD-Karte zu initialisieren
	SdSpiConfig spiConfig(chipSelect, DEDICATED_SPI, SD_SCK_MHZ(5));  // Reduzierte SPI-Geschwindigkeit für bessere Stabilität

	if (!sd.begin(spiConfig)) {
		Serial.println("Externer Speicher konnte nicht initialisiert werden. Versuch, sie zu formatieren...");
		delay(500);
		formatCard(spiConfig);  // Rufe die Formatierungsfunktion auf, wenn die Karte nicht initialisiert werden kann
	} else {
		Serial.println("Externer Speicher erfolgreich initialisiert.");
		writeToFile();  // Schreibe Daten auf die SD-Karte
		delay(1000);
		readFromFile();  // Lese die geschriebenen Daten
	}

	// Erstelle einen separaten Task für den Webserver
	xTaskCreate(webServerTask,    // Funktion, die der Task ausführt
	            "WebServerTask",  // Name des Tasks
	            8192,             // Stack-Größe in Bytes
	            NULL,             // Parameter für die Funktion (nicht benötigt)
	            1,                // Priorität des Tasks (niedriger Wert für niedrigere Priorität)
	            NULL              // Task-Handle (nicht benötigt)
	);
}

void loop() {
	// put your main code here, to run repeatedly:
}
