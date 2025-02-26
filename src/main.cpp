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
#include "handlers/log.handler.h"
#include "handlers/preferences.handler.h"
#include "handlers/response.handler.h"
#include "handlers/serial.handler.h"
#include "handlers/status.handler.h"
#include "handlers/system.handler.h"
#include "handlers/wifi.handler.h"

// SD-Karten-Pins (abhängig von deinem Board)
#define SD_CS_PIN 5  // Beispiel für Lolin32 Lite

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
	setupStatusSystem();  // Status-LEDs initialisieren

	// CPU-Frequenz auf 240 MHz setzen
	setCpuFrequencyMhz(240);

	logger.info("System startet...");

	preferences.begin("system", true);  // Namespace im Lesemodus öffnen
	LLog::debugLog = preferences.getBool("debug", false);
	preferences.end();

	// Initialisierung von SD-Karte
	bool sdCardOk = SD.begin(SD_CS_PIN);
	if (!sdCardOk) {
		logger.error("SD-Karte konnte nicht initialisiert werden!");
		updateStatus(STATUS_NO_SD_CARD);
	} else {
		logger.info("SD-Karte erfolgreich initialisiert.");
	}

	// Verzeichnisprüfung
	bool logsExists = SD.exists("/logs");
	bool htmlExists = SD.exists("/www/html");
	if (!htmlExists) {
		logger.error("Verzeichnis /www/html nicht gefunden!");
		updateStatus(STATUS_NO_HTML_DIR);
	}
	if (!logsExists) {
		logger.error("Verzeichnis /logs nicht gefunden!");
		updateStatus(STATUS_NO_LOGS_DIR);
		SD.mkdir("/logs");
	}

	// **Status-LEDs aktualisieren**
	if (!sdCardOk) {
		updateStatus(STATUS_NO_SD_CARD);
	} else if (!htmlExists) {
		updateStatus(STATUS_NO_HTML_DIR);
	} else if (!logsExists) {
		updateStatus(STATUS_NO_LOGS_DIR);
	} else {
		updateStatus(STATUS_READY);
	}

	// **Serielle Befehlswarteschlange initialisieren**
	commandQueue = xQueueCreate(10, sizeof(String));
	if (commandQueue == NULL) {
		logger.error("Queue konnte nicht erstellt werden!");
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

	logger.info("Access Point gestartet! IP-Adresse: " + WiFi.softAPIP().toString());

	// Event-Handler für AP-Clients registrieren
	WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
		switch (event) {
			case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
				logger.info("Neues Gerät mit AP verbunden.");
				updateStatus(STATUS_DEVICE_CONNECTED);
				break;
			case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
				logger.info("Gerät hat den AP verlassen.");
				updateStatus(STATUS_NO_DEVICE_CONNECTED);
				break;
			default:
				break;
		}
	});

	// STA-Modus prüfen und verbinden
	String ssid, password;
	if (isWiFiEnabled && readWiFiConfig(ssid, password)) {
		connectToWiFi(ssid, password);
	} else {
		logger.warn("[WLAN] WLAN-Konfigurationsdatei fehlt oder ist leer. Keine Verbindung hergestellt.");
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

	// **AP-Status aktualisieren (wenn sich die Anzahl der verbundenen Clients ändert)**
	static int lastConnectedClients = WiFi.softAPgetStationNum();
	int connectedClients = WiFi.softAPgetStationNum();

	if (connectedClients != lastConnectedClients) {
		if (connectedClients > 0) {
			logger.info("Gerät mit AP verbunden.");
			updateStatus(STATUS_DEVICE_CONNECTED);
		} else {
			logger.warn("Kein Gerät mit AP verbunden.");
			updateStatus(STATUS_NO_DEVICE_CONNECTED);
		}
		lastConnectedClients = connectedClients;
	}

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
