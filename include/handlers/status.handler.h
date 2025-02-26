#ifndef STATUS_HANDLER_H
#define STATUS_HANDLER_H

#include <Arduino.h>

// LED-Pins (angepasst an dein Board)
#define RED_LED 33
#define GREEN_LED 32
#define YELLOW_LED 25

// Status-Codes für das System
enum SystemStatus {
	STATUS_INITIALIZING,
	STATUS_READY,
	STATUS_NO_SD_CARD,
	STATUS_NO_HTML_DIR,
	STATUS_NO_LOGS_DIR,
	STATUS_LOG_FILE_ERROR,
	STATUS_LOG_WRITE,
	STATUS_NO_DEVICE_CONNECTED,
	STATUS_DEVICE_CONNECTED
};

// Funktionen zur Statusverwaltung
void setupStatusSystem();
void updateStatus(SystemStatus status);
void logWriteBlink();

#endif
