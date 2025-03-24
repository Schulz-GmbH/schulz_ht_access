#ifndef STATUS_HANDLER_H
#define STATUS_HANDLER_H

#include <WiFi.h>

#include <vector>

#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "global.h"

// Status-Codes für das System
enum SystemStatus {
	SYSTEM_INITIALIZING,
	SYSTEM_READY,
	// SD-Karten-Status
	SD_CARD_NOT_AVAILABLE,
	// Log-Status
	LOG_NO_DIR,
	LOG_FILE_ERROR,
	LOG_WRITE,
	// Webserver-Status
	WEBSERVER_NO_HTML_DIR,
	// WLAN-Status
	WIFI_STA_NOT_AVAILABLE,
	WIFI_AP_NOT_AVAILABLE,
	WIFI_AP_NO_DEVICE,
	WIFI_AP_DEVICE_AVAILABLE,
	// Serial-Status
	SERIAL_NOT_CONNECTED,
	SERIAL_CONNECTED,
	SERIAL_SEND,

	/*
	 * Weitere Status hier ergänzen,
	 * falls z.B. andere Fehlerzustände angezeigt werden sollen
	 */
};

// Funktionen zur Statusverwaltung
void startStatusSystem();
void addStatus(SystemStatus status);
void removeStatus(SystemStatus status);

#endif
