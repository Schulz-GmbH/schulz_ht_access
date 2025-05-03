// SerialBridge.h
#ifndef SERIALBRIDGE_H
#define SERIALBRIDGE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "LLog.h"

class SerialBridge {
   public:
	SerialBridge(HardwareSerial &serial, AsyncWebSocket &ws, uint8_t rxPin, uint8_t txPin);

	// Start serial port at baud and initialize pins
	void begin(uint32_t baud);

	// Create task to poll serial, notify availability/data
	void start(TaskHandle_t *taskHandle, UBaseType_t priority = 3, BaseType_t core = 1);

	// Change baud rate on the fly and notify
	bool setBaud(uint32_t newBaud);

	// Query current connection and baud
	bool isDeviceConnected() const;
	uint32_t getBaudRate() const;

	// Send current availability/baud to all clients
	void sendAvailability();

   private:
	HardwareSerial &_serial;
	AsyncWebSocket &_ws;
	uint8_t _rxPin, _txPin;
	uint32_t _baudRate;
	bool _deviceConnected;

	// Für das Batch‐Sammeln
	static constexpr uint32_t BATCH_TIMEOUT_MS = 20;
	char _batchBuffer[256];
	size_t _batchIndex;
	uint32_t _lastRx;

	static void taskFunc(void *param);
};

#endif  // SERIALBRIDGE_H
