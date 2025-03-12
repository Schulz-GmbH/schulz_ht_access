#ifndef STATUS_HANDLER_H
#define STATUS_HANDLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "global.h"

extern TaskHandle_t blinkTaskHandle;
extern TaskHandle_t initBlinkTaskHandle;

// Status-Codes für das System
enum SystemStatus {
	STATUS_INITIALIZING,
	STATUS_READY,
};

// Funktionen zur Statusverwaltung
void setupStatusSystem();
void updateStatus(SystemStatus status);
void logWriteBlink();

#endif
