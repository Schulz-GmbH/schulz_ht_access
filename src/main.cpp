#include <global.h>

#include "LLog.h"
#include "SDCard.h"
#include "StatusHandler.h"

// Instanziierung der Preferences
Preferences preferences;

void setup() {
	// CPU-Frequenz auf 240 MHz setzen
	setCpuFrequencyMhz(240);

	Serial.begin(9600);
	logger.info("System startet...");

	preferences.begin("system", false);
	// LLog::active = preferences.getBool("debug", false);
	LLog::setActive(true);
	preferences.end();

	setupStatusSystem();
}

void loop() {
}
