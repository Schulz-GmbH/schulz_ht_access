#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <MDNS.h>
#include <SD.h>   //einbinden SD-Karten Bibliothek
#include <SPI.h>  //einbinden SPI Bibliothek

#include "handlers/card.handler.h"
#include "handlers/cors.handler.h"
#include "handlers/wifi.handler.h"
#include "routes/routes.h"

const int chipSelect = 5;  // SD-Karten-Pin

// Netzwerk Konfiguration für AP
const char *apSSID = "ESP32-AccessPoint";
const char *apPassword = "12345678";

// Webserver auf Port 80 initialisieren
AsyncWebServer server(80);

// Mutex für SD-Karten Zugriff
SemaphoreHandle_t sdMutex;

// Funktion, die den Webserver startet
void startWebServer() {
	// CORS konfigurieren
	setupCORS(server);
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

void listSDContents() {
	File root = SD.open("/");
	if (!root) {
		Serial.println("Fehler beim Öffnen des Root-Verzeichnisses.");
		return;
	}

	if (!root.isDirectory()) {
		Serial.println("Root ist kein Verzeichnis.");
		return;
	}

	File file = root.openNextFile();
	while (file) {
		Serial.print("DATEI: ");
		Serial.print(file.name());
		if (file.isDirectory()) {
			Serial.println(" (Verzeichnis)");
		} else {
			Serial.print("\tGRÖSSE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}

void setup() {
	// Serielle Kommunikation starten
	Serial.begin(9600);

	// Mutex erstellen
	sdMutex = xSemaphoreCreateMutex();
	if (sdMutex == NULL) {
		Serial.println("Fehler beim Erstellen des Mutex");
		return;
	}

	Serial.println("Initialisiere externen Speicher...");
	delay(1000);  // Eine kurze Verzögerung vor dem Initialisieren kann helfen

	// SdSpiConfig erstellen, um die SD-Karte zu initialisieren
	SdSpiConfig spiConfig(chipSelect, DEDICATED_SPI, SD_SCK_MHZ(5));

	// SD-Karte initialisieren mit Mutex-Schutz
	if (xSemaphoreTake(sdMutex, portMAX_DELAY) == pdTRUE) {
		if (!SD.begin(chipSelect)) {
			Serial.println("Externer Speicher konnte nicht initialisiert werden.");
			switch (SD.cardType()) {
				case CARD_NONE:
					Serial.println("Fehler: Keine SD-Karte erkannt.");
					Serial.println("Überprüfen Sie, ob die SD-Karte richtig eingesteckt ist.");
					break;
				case CARD_MMC:
					Serial.println("Fehler: Es wurde eine MMC-Karte erkannt, die möglicherweise nicht kompatibel ist.");
					break;
				case CARD_SD:
					Serial.println("SD-Karte erkannt, aber ein anderer Fehler verhindert die Initialisierung.");
					Serial.println(
					    "Mögliche Ursachen könnten eine falsche Verdrahtung, ein Timing-Problem oder eine unzureichende Stromversorgung sein.");
					break;
				case CARD_SDHC:
					Serial.println("SDHC-Karte erkannt, aber ein anderer Fehler verhindert die Initialisierung.");
					Serial.println(
					    "Mögliche Ursachen könnten eine falsche Verdrahtung, ein Timing-Problem oder eine unzureichende Stromversorgung sein.");
					break;
				default:
					Serial.println("Unbekannter Fehler bei der Initialisierung.");
					break;
			}
			xSemaphoreGive(sdMutex);
		} else {
			Serial.println("Externer Speicher erfolgreich initialisiert.");
			Serial.println("Liste alle Daten im externen Speicher auf.");
			listSDContents();
		}
		// Gebe den Mutex wieder frei
		xSemaphoreGive(sdMutex);
	} else {
		Serial.println("Konnte Mutex nicht nehmen");
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
}
