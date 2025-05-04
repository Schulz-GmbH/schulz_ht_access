/**
 * @file SerialBridge.cpp
 * @brief Brücke zwischen einer HardwareSerial-Schnittstelle und einem AsyncWebSocket.
 *
 * Diese Klasse ermöglicht die serielle Kommunikation mit einem angeschlossenen Gerät
 * über eine UART-Verbindung und leitet empfangene Daten als JSON-formatiertes Event
 * über einen WebSocket an verbundene Clients weiter.
 *
 * Zusätzlich erkennt sie automatisch, ob ein Gerät verbunden ist (basierend auf RX/TX),
 * ermöglicht die dynamische Änderung der Baudrate und puffert empfangene Zeichen.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */
#include "SerialBridge.h"

#include "global.h"

/**
 * @brief Konstruktor der SerialBridge-Klasse.
 *
 * @param serial Referenz auf die verwendete HardwareSerial-Instanz.
 * @param ws Referenz auf die WebSocket-Instanz.
 * @param rxPin Der RX-Pin (Empfang).
 * @param txPin Der TX-Pin (Senden).
 */
SerialBridge::SerialBridge(HardwareSerial &serial, AsyncWebSocket &ws, uint8_t rxPin, uint8_t txPin)
    : _serial(serial), _ws(ws), _rxPin(rxPin), _txPin(txPin), _baudRate(0), _deviceConnected(false), _batchIndex(0), _lastRx(0) {
	pinMode(_rxPin, INPUT);
	pinMode(_txPin, OUTPUT);
}

/**
 * @brief Initialisiert die serielle Verbindung mit gegebener Baudrate.
 *
 * @param baud Baudrate zur Initialisierung der seriellen Schnittstelle.
 */
void SerialBridge::begin(uint32_t baud) {
	_baudRate = baud;
	_serial.begin(_baudRate, SERIAL_8N1, _rxPin, _txPin);
	sendAvailability();
}

/**
 * @brief Startet die FreeRTOS-Task zur permanenten Überwachung und Verarbeitung.
 *
 * @param taskHandle Optionaler Zeiger auf das Task-Handle.
 * @param priority Priorität der Task.
 * @param core CPU-Core, auf dem die Task laufen soll.
 */
void SerialBridge::start(TaskHandle_t *taskHandle, UBaseType_t priority, BaseType_t core) {
	xTaskCreatePinnedToCore(taskFunc, "SerialBridgeTask", 4096, this, priority, taskHandle, core);
}

/**
 * @brief Setzt eine neue Baudrate, falls sie gültig ist.
 *
 * @param newBaud Neue Baudrate als uint32_t.
 * @return true, wenn erfolgreich gesetzt oder identisch.
 */
bool SerialBridge::setBaud(uint32_t newBaud) {
	if (newBaud != _baudRate && isValidBaudRate(String(newBaud))) {
		_baudRate = newBaud;
		_serial.begin(_baudRate, SERIAL_8N1, _rxPin, _txPin);
	}
	sendAvailability();
	return true;
}

/**
 * @brief Prüft, ob eine übergebene Baudrate in der erlaubten Liste enthalten ist.
 *
 * @param value Baudrate als String.
 * @return true, wenn gültig; sonst false.
 */
bool SerialBridge::isValidBaudRate(const String &value) {
	int rate = value.toInt();
	for (size_t i = 0; i < NUM_BAUD_RATES; ++i)
		if (rate == baudRates[i]) return true;
	return false;
}

/**
 * @brief Gibt an, ob derzeit ein Gerät über RX/TX erkannt wurde.
 *
 * @return true, wenn Gerät verbunden; sonst false.
 */
bool SerialBridge::isDeviceConnected() const {
	return _deviceConnected;
}

/**
 * @brief Gibt die aktuelle Baudrate zurück.
 *
 * @return Baudrate als uint32_t.
 */
uint32_t SerialBridge::getBaudRate() const {
	return _baudRate;
}

/**
 * @brief Sendet die aktuelle Verfügbarkeit und Baudrate an alle WebSocket-Clients.
 */
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

/**
 * @brief Sendet Daten direkt über die serielle Schnittstelle.
 *
 * @param data Der zu sendende String.
 */
void SerialBridge::sendData(const String &data) {
	// direkt auf der HardwareSerial ausgeben
	_serial.print(data);
}

/**
 * @brief Interne FreeRTOS-Task-Funktion zur Überwachung des seriellen Eingangs.
 *
 * Diese Funktion wird dauerhaft ausgeführt. Sie prüft:
 * - ob ein Gerät verbunden ist (über RX/TX-Leitungen),
 * - ob neue Zeichen empfangen wurden (und sendet sie gebündelt weiter),
 * - ob nach einer definierten Zeit (BATCH_TIMEOUT_MS) die aktuellen Daten geflusht werden.
 *
 * @param param Pointer auf die SerialBridge-Instanz (this).
 */
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
				logger.log({"serial", "info", "device"}, String(self->_batchBuffer));
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
