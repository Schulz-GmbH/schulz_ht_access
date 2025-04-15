#include "handlers/serial.handler.h"

#include <HardwareSerial.h>

#include "freertos/queue.h"
#include "handlers/response.handler.h"

extern HardwareSerial SerialDevice;
extern QueueHandle_t commandQueue;

bool consoleRunning = false;

void handleSerialEvent(const String &setting, const String &value, AsyncWebSocketClient *client) {
	if (setting == "set") {
		int baudRate = value.toInt();
		if (baudRate > 0) {
			SerialDevice.begin(baudRate, SERIAL_8N1, RXD2, TXD2);  // RXD2, TXD2
			consoleRunning = true;
			sendResponse(client, "serial", "set", "success", "Baudrate gesetzt: " + String(baudRate));
		} else {
			consoleRunning = false;
			sendResponse(client, "serial", "set", "error", "", "invalid baud rate");
		}
	} else if (setting == "disconnect") {
		if (consoleRunning) {
			SerialDevice.end();  // Serielle Verbindung beenden
			consoleRunning = false;
			sendResponse(client, "serial", "disconnect", "success", "Serial connection closed");
		} else {
			sendResponse(client, "serial", "disconnect", "error", "", "no active serial connection");
		}
	} else if (setting == "send") {
		if (xQueueSend(commandQueue, &value, pdMS_TO_TICKS(100)) != pdPASS) {
			sendResponse(client, "serial", "send", "error", "", "queue full");
		} else {
			sendResponse(client, "serial", "send", "success", "Command added to queue: " + value);
		}
	} else {
		consoleRunning = false;
		sendResponse(client, "serial", "error", "unknown", "", "unknown serial setting");
	}
}
