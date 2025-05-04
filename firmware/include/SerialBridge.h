/**
 * @file SerialBridge.h
 * @brief Header für die Klasse SerialBridge zur Kommunikation über HardwareSerial und WebSocket.
 *
 * Diese Klasse stellt eine Brücke zwischen einer seriellen Schnittstelle (UART) und
 * einem WebSocket dar. Sie erlaubt die Übertragung empfangener serieller Daten an
 * alle verbundenen WebSocket-Clients und bietet Funktionen zur Verwaltung der Baudrate,
 * Geräteverbindung sowie zur Statusübermittlung.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#ifndef SERIALBRIDGE_H
#define SERIALBRIDGE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "LLog.h"

/**
 * @class SerialBridge
 * @brief Klasse zur Kopplung eines seriellen Geräts mit einem WebSocket-Client.
 *
 * Ermöglicht das Lesen und Schreiben serieller Daten sowie die Weiterleitung
 * an WebSocket-Clients. Unterstützt dynamische Baudratenänderung, Gerätezustandserkennung
 * und gepufferte Datenübertragung.
 */
class SerialBridge {
   public:
	/**
	 * @brief Konstruktor.
	 *
	 * @param serial Referenz auf die HardwareSerial-Instanz.
	 * @param ws Referenz auf die WebSocket-Instanz.
	 * @param rxPin Pin für RX (Empfang).
	 * @param txPin Pin für TX (Senden).
	 */
	SerialBridge(HardwareSerial &serial, AsyncWebSocket &ws, uint8_t rxPin, uint8_t txPin);

	/**
	 * @brief Initialisiert die serielle Schnittstelle mit der angegebenen Baudrate.
	 *
	 * @param baud Die Baudrate, mit der die Schnittstelle konfiguriert wird.
	 */
	void begin(uint32_t baud);

	/**
	 * @brief Startet die Task zur kontinuierlichen Überwachung der seriellen Schnittstelle.
	 *
	 * @param taskHandle Optionaler Zeiger zum Erhalt des TaskHandles.
	 * @param priority Priorität der Task (Default: 3).
	 * @param core CPU-Core, auf dem die Task laufen soll (Default: 1).
	 */
	void start(TaskHandle_t *taskHandle, UBaseType_t priority = 3, BaseType_t core = 1);

	/**
	 * @brief Setzt eine neue Baudrate und initialisiert die Schnittstelle neu.
	 *
	 * @param newBaud Neue Baudrate.
	 * @return true, wenn gültig und gesetzt; andernfalls false.
	 */
	bool setBaud(uint32_t newBaud);

	/**
	 * @brief Prüft, ob eine gegebene Baudrate zulässig ist.
	 *
	 * @param value Baudrate als String.
	 * @return true, wenn gültig; andernfalls false.
	 */
	bool isValidBaudRate(const String &value);

	/**
	 * @brief Prüft, ob aktuell ein serielles Gerät angeschlossen ist.
	 *
	 * @return true, wenn verbunden; andernfalls false.
	 */
	bool isDeviceConnected() const;

	/**
	 * @brief Gibt die aktuell verwendete Baudrate zurück.
	 *
	 * @return Aktuelle Baudrate.
	 */
	uint32_t getBaudRate() const;

	/**
	 * @brief Sendet Verfügbarkeitsinformationen und Baudrate an alle WebSocket-Clients.
	 */
	void sendAvailability();

	/**
	 * @brief Sendet Daten über die serielle Schnittstelle.
	 *
	 * @param data Der zu sendende String.
	 */
	void sendData(const String &data);

   private:
	HardwareSerial &_serial;  ///< Referenz auf die serielle Schnittstelle
	AsyncWebSocket &_ws;      ///< Referenz auf den WebSocket-Server
	uint8_t _rxPin, _txPin;   ///< RX- und TX-Pin
	uint32_t _baudRate;       ///< Aktuelle Baudrate
	bool _deviceConnected;    ///< Status der Geräteverbindung

	static constexpr uint32_t BATCH_TIMEOUT_MS = 20;  ///< Timeout (ms) für Batch-Verarbeitung
	char _batchBuffer[256];                           ///< Puffer für empfangene Zeichen
	size_t _batchIndex;                               ///< Aktuelle Position im Batch-Puffer
	uint32_t _lastRx;                                 ///< Zeitstempel des letzten Zeicheneingangs

	/**
	 * @brief Interne Task-Funktion für FreeRTOS zur seriellen Datenverarbeitung.
	 *
	 * @param param Zeiger auf die SerialBridge-Instanz.
	 */
	static void taskFunc(void *param);
};

#endif  // SERIALBRIDGE_H
