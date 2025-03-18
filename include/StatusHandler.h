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
	STATUS_INITIALIZING,
	STATUS_READY,
	STATUS_NO_SD_CARD,
	STATUS_NO_HTML_DIR,
	STATUS_NO_LOGS_DIR,
	STATUS_WIFI_NOT_AVAILABLE,
	STATUS_NO_WIFI_DEVICE
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
