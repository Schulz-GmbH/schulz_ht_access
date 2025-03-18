#ifndef STATUS_HANDLER_H
#define STATUS_HANDLER_H

#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "global.h"

// Status-Codes für das System
enum SystemStatus {
	STATUS_INITIALIZING,
	STATUS_READY,
	/*
	 * Weitere Status hier ergänzen,
	 * falls z.B. andere Fehlerzustände angezeigt werden sollen
	 */
};

// Funktionen zur Statusverwaltung
void setupStatusSystem();
void addStatus(SystemStatus status);
void removeStatus(SystemStatus status);

#endif
