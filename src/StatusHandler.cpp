#include "StatusHandler.h"

TaskHandle_t blinkTaskHandle = NULL;
TaskHandle_t initBlinkTaskHandle = NULL;

/**
 * Initialisiert die Status-LEDs.
 */
void setupStatusSystem() {
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);

	digitalWrite(RED_LED, LOW);
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	// Starte das Blinken der grünen LED während der Initialisierung
	xTaskCreate(
	    [](void *param) {
		    while (true) {
			    digitalWrite(GREEN_LED, HIGH);
			    vTaskDelay(pdMS_TO_TICKS(300));
			    digitalWrite(GREEN_LED, LOW);
			    vTaskDelay(pdMS_TO_TICKS(300));
		    }
	    },
	    "InitBlinkTask", 1024, NULL, 1, &initBlinkTaskHandle);
}

/**
 * Setzt den aktuellen Systemstatus und aktualisiert die LEDs entsprechend.
 */
void updateStatus(SystemStatus status) {
	static SystemStatus lastStatus = STATUS_INITIALIZING;

	// Falls der Status sich nicht geändert hat, nichts tun
	if (status == lastStatus) return;
	lastStatus = status;

	// Falls ein Blink-Task aktiv ist, stoppen
	if (blinkTaskHandle != NULL) {
		vTaskDelete(blinkTaskHandle);
		blinkTaskHandle = NULL;
	}

	// Falls das Initialisierungs-Blinken noch läuft, stoppen
	if (initBlinkTaskHandle != NULL) {
		vTaskDelete(initBlinkTaskHandle);
		initBlinkTaskHandle = NULL;
		digitalWrite(GREEN_LED, LOW);  // Grüne LED aus
	}

	// Zuerst alle LEDs ausschalten
	digitalWrite(RED_LED, LOW);
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	switch (status) {
		case STATUS_INITIALIZING:
			// Die Initialisierung wird bereits durch das grüne Blinken dargestellt
			break;
		case STATUS_READY:
			digitalWrite(GREEN_LED, HIGH);  // Grün EIN → System bereit
			break;
	}
}
