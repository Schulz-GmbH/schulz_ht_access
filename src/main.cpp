#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

// Handler-Includes
#include "handlers/light.handler.h"
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

// WLAN-Konfiguration
const char *ap_ssid = "ESP32-AccessPoint";
const char *ap_password = "12345678";

void processSerialQueue(void *param) {
	String command;
	while (true) {
		if (xQueueReceive(commandQueue, &command, portMAX_DELAY) == pdPASS) {
			SerialDevice.println(command);
			SerialDevice.flush();
			Serial.printf("Befehl an SerialDevice gesendet: %s\n", command.c_str());
		}
	}
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	switch (type) {
		case WS_EVT_CONNECT:
			Serial.printf("WebSocket-Client %u verbunden.\n", client->id());
			break;
		case WS_EVT_DISCONNECT:
			Serial.printf("WebSocket-Client %u getrennt.\n", client->id());
			break;
		case WS_EVT_DATA: {
			AwsFrameInfo *info = (AwsFrameInfo *)arg;
			if (info->opcode == WS_TEXT) {
				String message = String((char *)data).substring(0, len);
				Serial.printf("WebSocket Nachricht empfangen: %s\n", message.c_str());

				// JSON-Daten parsen
				StaticJsonDocument<512> jsonDoc;
				DeserializationError error = deserializeJson(jsonDoc, message);

				if (error) {
					Serial.printf("JSON Parsing Error: %s\n", error.c_str());
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
			Serial.printf("WebSocket-Fehler bei Client %u.\n", client->id());
			break;
		case WS_EVT_PONG:
			Serial.printf("WebSocket-Ping/Pong von Client %u.\n", client->id());
			break;
	}
}

void setup() {
	Serial.begin(9600);

	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);

	digitalWrite(RED_LED, LOW);
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	if (!SD.begin(SD_CS_PIN)) {
		Serial.println("Fehler beim Initialisieren der SD-Karte!");
		return;
	} else {
		Serial.println("SD-Karte erfolgreich initialisiert.");
	}

	if (!SD.exists("/www/html")) {
		Serial.println("Verzeichnis /www/html auf der SD-Karte nicht gefunden!");
		return;
	}

	commandQueue = xQueueCreate(10, sizeof(String));
	if (commandQueue == NULL) {
		Serial.println("Fehler: Queue konnte nicht erstellt werden!");
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
	}

	ws.onEvent(onEvent);
	server.addHandler(&ws);
	server.serveStatic("/", SD, "/www/html/").setDefaultFile("index.html");
	server.begin();

	Serial.println("WebSocket-Server gestartet.");
}

void loop() {
	ws.cleanupClients();

	if (consoleRunning && SerialDevice.available()) {
		char received = SerialDevice.read();
		Serial.write(received);
		String receivedStr = String(received);
		ws.textAll(receivedStr);
	}
}
