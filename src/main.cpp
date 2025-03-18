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

	// SD-Karte initialisieren (legt ggf. Status in Statusliste)
	initSDCard();
}

void loop() {
	// vTaskDelay(pdMS_TO_TICKS(20000));
	// removeStatus(STATUS_INITIALIZING);

	// Hier fortlaufend prüfen, ob SD-Karte & Ordner noch da sind.
	// Je nach Anforderung z.B. alle paar Sekunden:
	static unsigned long lastCheck = 0;
	if (millis() - lastCheck >= 5000) {  // alle 5 Sekunden
		lastCheck = millis();
		checkSDCard();
	}

	// Hier kann restlicher Programm-Code hin.
	delay(10);

	// Testausgaben
}
