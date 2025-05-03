// SerialBridge.cpp
#include "SerialBridge.h"

#include "global.h"

SerialBridge::SerialBridge(HardwareSerial &serial, AsyncWebSocket &ws, uint8_t rxPin, uint8_t txPin)
    : _serial(serial), _ws(ws), _rxPin(rxPin), _txPin(txPin), _baudRate(0), _deviceConnected(false), _batchIndex(0), _lastRx(0) {
	pinMode(_rxPin, INPUT);
	pinMode(_txPin, OUTPUT);
}

void SerialBridge::begin(uint32_t baud) {
	_baudRate = baud;
	_serial.begin(_baudRate, SERIAL_8N1, _rxPin, _txPin);
	sendAvailability();
}

void SerialBridge::start(TaskHandle_t *taskHandle, UBaseType_t priority, BaseType_t core) {
	xTaskCreatePinnedToCore(taskFunc, "SerialBridgeTask", 4096, this, priority, taskHandle, core);
}

bool SerialBridge::setBaud(uint32_t newBaud) {
	if (newBaud != _baudRate) {
		_baudRate = newBaud;
		_serial.begin(_baudRate, SERIAL_8N1, _rxPin, _txPin);
	}
	sendAvailability();
	return true;
}

bool SerialBridge::isDeviceConnected() const {
	return _deviceConnected;
}

uint32_t SerialBridge::getBaudRate() const {
	return _baudRate;
}

void SerialBridge::sendAvailability() {
	StaticJsonDocument<256> doc;
	doc["event"] = "serial";
	doc["action"] = "status";
	doc["status"] = "success";
	JsonObject details = doc.createNestedObject("details");
	details["available"] = _deviceConnected;
	details["baudRate"] = _baudRate;
	String payload;
	serializeJson(doc, payload);
	_ws.textAll(payload);
}

void SerialBridge::taskFunc(void *param) {
	auto *self = static_cast<SerialBridge *>(param);
	self->_batchIndex = 0;
	self->_lastRx = millis();

	for (;;) {
		int rxState = digitalRead(RXD2);
		int txState = digitalRead(TXD2);

		if (!self->_deviceConnected && (rxState == HIGH && txState == HIGH)) {
			self->_deviceConnected = true;
			self->sendAvailability();
			logger.log({"system", "info", "device"}, "Device connected");
		}

		// 1) Neue Bytes einsammeln
		while (self->_serial.available()) {
			char c = self->_serial.read();
			uint32_t now = millis();
			self->_lastRx = now;

			// Zeichen in den Puffer
			if (self->_batchIndex + 1 < sizeof(self->_batchBuffer)) {
				self->_batchBuffer[self->_batchIndex++] = c;
			}

			// SOFORT schicken, wenn wir einen Zeilenumbruch haben
			if (c == '\r' || c == '\n' || self->_batchIndex + 1 == sizeof(self->_batchBuffer)) {
				self->_batchBuffer[self->_batchIndex] = '\0';
				StaticJsonDocument<256> doc;
				doc["event"] = "serial";
				doc["action"] = "incoming";
				doc["status"] = "data";
				doc["details"] = self->_batchBuffer;
				String payload;
				serializeJson(doc, payload);
				self->_ws.textAll(payload);
				self->_batchIndex = 0;
			}
		}

		// 2) Wenn nach BATCH_TIMEOUT_MS keine neuen Bytes kamen, flushen
		uint32_t since = millis() - self->_lastRx;
		if (self->_batchIndex > 0 && since >= SerialBridge::BATCH_TIMEOUT_MS) {
			self->_batchBuffer[self->_batchIndex] = '\0';
			StaticJsonDocument<256> doc;
			doc["event"] = "serial";
			doc["action"] = "incoming";
			doc["status"] = "data";
			doc["details"] = self->_batchBuffer;
			String payload;
			serializeJson(doc, payload);
			self->_ws.textAll(payload);
			self->_batchIndex = 0;
		}

		vTaskDelay(pdMS_TO_TICKS(5));
	}
}
