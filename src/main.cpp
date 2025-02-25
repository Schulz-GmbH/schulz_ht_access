#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>

#include <ctime>  // Zeitstempel für die Log-Datei

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

// Handler-Includes
#include "handlers/light.handler.h"
#include "handlers/log.handler.h"
#include "handlers/preferences.handler.h"
#include "handlers/response.handler.h"
#include "handlers/serial.handler.h"
#include "handlers/system.handler.h"
#include "handlers/wifi.handler.h"

// SD-Karten-Pins (abhängig von deinem Board)
#define SD_CS_PIN 5  // Beispiel für Lolin32 Lite

// LED-Pins
const int RED_LED = 33;
const int GREEN_LED = 32;
const int YELLOW_LED = 25;

TaskHandle_t redBlinkTaskHandle = NULL;
TaskHandle_t greenBlinkTaskHandle = NULL;
TaskHandle_t yellowBlinkTaskHandle = NULL;

HardwareSerial SerialDevice(1);  // UART2
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
QueueHandle_t commandQueue;
File logFile;  // Log-Datei

// Instanziierung der Preferences
Preferences preferences;

// WLAN-Konfiguration
const char *ap_ssid = "ESP32-AccessPoint";
const char *ap_password = "12345678";

void processSerialQueue(void *param) {
	String command;
	while (true) {
		if (xQueueReceive(commandQueue, &command, portMAX_DELAY) == pdPASS) {
			SerialDevice.println(command);
			SerialDevice.flush();
			logger.info("Befehl an SerialDevice gesendet: " + command);
		}
	}
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	switch (type) {
		case WS_EVT_CONNECT:
			logger.info("WebSocket-Client " + String(client->id()) + " verbunden.");
			// startLogging();
			break;
		case WS_EVT_DISCONNECT:
			logger.info("WebSocket-Client " + String(client->id()) + " getrennt.");
			// stopLogging();
			break;
		case WS_EVT_DATA: {
			AwsFrameInfo *info = (AwsFrameInfo *)arg;
			if (info->opcode == WS_TEXT) {
				String message = String((char *)data).substring(0, len);
				logger.info("WebSocket Nachricht empfangen: " + message);

				// JSON-Daten parsen
				StaticJsonDocument<512> jsonDoc;
				DeserializationError error = deserializeJson(jsonDoc, message);

				if (error) {
					logger.error("JSON Parsing Error: " + String(error.c_str()));
					client->text("{\"event\":\"error\",\"details\":\"Invalid JSON format\"}");
					return;
				}

				// JSON-Felder auslesen
				String command = jsonDoc["command"] | "";
				String setting = jsonDoc["setting"] | "";
				String value = jsonDoc["value"] | "";

				// Befehle basierend auf dem "command"-Feld verarbeiten
				if (command == "wifi") {
					handleWiFiEvent(setting, value, client);
				} else if (command == "serial") {
					handleSerialEvent(setting, value, client);
				} else if (command == "log") {
					handleLogEvent(setting, value, client);
				} else if (command == "system") {
					handleSystemEvent(setting, value, client);
				} else if (command == "light") {
					handleLightEvent(setting, value, client);
				} else {
					sendResponse(client, "unknown", "error", "unknown command", "", "Invalid command");
				}
			}
			break;
		}
		case WS_EVT_ERROR:
			logger.error("WebSocket-Fehler bei Client " + String(client->id()) + ".");
			break;
		case WS_EVT_PONG:
			logger.info("WebSocket-Ping/Pong von Client " + String(client->id()) + ".");
			break;
	}
}

void setup() {
	Serial.begin(9600);

	// CPU-Frequenz auf 240 MHz setzen
	setCpuFrequencyMhz(240);

	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);

	digitalWrite(RED_LED, LOW);
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	if (!SD.begin(SD_CS_PIN)) {
		logger.error("Fehler beim Initialisieren der SD-Karte!");
		return;
	} else {
		logger.info("SD-Karte erfolgreich initialisiert.");
	}

	if (!SD.exists("/www/html")) {
		logger.error("Verzeichnis /www/html auf der SD-Karte nicht gefunden!");
		return;
	} else if (!SD.exists("/logs")) {
		logger.error("Verzeichnis /logs auf der SD-Karte nicht gefunden!");
		SD.mkdir("/logs");  // Erstellt das Verzeichnis, falls nicht vorhanden
	}

	preferences.begin("system", true);  // Namespace im Lesemodus öffnen
	LLog::debugLog = preferences.getBool("debug", false);
	preferences.end();

	commandQueue = xQueueCreate(10, sizeof(String));
	if (commandQueue == NULL) {
		logger.debug("Queue konnte nicht erstellt werden!");
		while (true);
	}

	xTaskCreate(processSerialQueue, "ProcessSerialQueue", 2048, NULL, 1, NULL);

	// WiFi Access Point starten
	IPAddress local_IP(192, 168, 5, 1);  // IP-Adresse des AP
	IPAddress gateway(192, 168, 5, 1);   // Gateway-Adresse (gleich der AP-IP)
	IPAddress subnet(255, 255, 255, 0);  // Subnetzmaske

	WiFi.mode(WIFI_AP_STA);

	WiFi.softAPConfig(local_IP, gateway, subnet);
	WiFi.softAP(ap_ssid, ap_password);

	Serial.printf("Access Point gestartet! IP-Adresse: %s\n", WiFi.softAPIP().toString().c_str());

	// STA-Modus prüfen und verbinden
	String ssid, password;
	if (isWiFiEnabled && readWiFiConfig(ssid, password)) {
		connectToWiFi(ssid, password);
	} else {
		Serial.println("WLAN-Konfigurationsdatei fehlt oder ist leer. Keine Verbindung hergestellt.");
		logger.wlan("[WLAN] WLAN-Konfigurationsdatei fehlt oder ist leer. Keine Verbindung hergestellt.");
	}

	ws.onEvent(onEvent);
	server.addHandler(&ws);
	server.serveStatic("/", SD, "/www/html/").setDefaultFile("index.html");

	// REST-Endpunkte dor dynamic log files
	server.on("/logfile", HTTP_GET, [](AsyncWebServerRequest *request) {
		logger.http("[HTTP] HTTP-Anfrage erhalten: /logfile");
		serveLogFile(request);
	});

	// Vue-Router Fallback für SPA
	server.onNotFound([](AsyncWebServerRequest *request) { request->send(SD, "/www/html/index.html", "text/html"); });
	server.begin();

	logger.info("WebSocket-Server gestartet.");
}

void loop() {
	ws.cleanupClients();

	if (consoleRunning && SerialDevice.available()) {
		// Daten von SerialDevice lesen
		String receivedData = "";
		while (SerialDevice.available()) {
			char received = SerialDevice.read();

			// Nur sichtbare Zeichen oder Zeilenumbrüche beibehalten
			if ((received >= 32 && received <= 126) || received == '\n' || received == '\r') {
				receivedData += received;
			}
		}

		logger.info(receivedData);

		// JSON-Antwort an alle verbundenen Clients senden
		for (AsyncWebSocketClient *client : ws.getClients()) {
			if (client && client->status() == WS_CONNECTED) {
				sendResponse(client, "serial", "receive", "success", receivedData, "");
			}
		}
	}
}
