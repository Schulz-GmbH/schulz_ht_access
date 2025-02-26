#include "handlers/status.handler.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
		case STATUS_NO_SD_CARD:
			digitalWrite(RED_LED, HIGH);  // Rot EIN → SD-Karte fehlt
			break;
		case STATUS_NO_HTML_DIR:
			for (int i = 0; i < 3; i++) {  // 3x Blink → HTML-Ordner fehlt
				digitalWrite(RED_LED, HIGH);
				vTaskDelay(pdMS_TO_TICKS(300));
				digitalWrite(RED_LED, LOW);
				vTaskDelay(pdMS_TO_TICKS(300));
			}
			break;
		case STATUS_NO_LOGS_DIR:
			for (int i = 0; i < 2; i++) {  // 2x Blink → Logs-Ordner fehlt
				digitalWrite(RED_LED, HIGH);
				vTaskDelay(pdMS_TO_TICKS(500));
				digitalWrite(RED_LED, LOW);
				vTaskDelay(pdMS_TO_TICKS(500));
			}
			break;
		case STATUS_LOG_FILE_ERROR:
			digitalWrite(GREEN_LED, HIGH);   // Grün EIN
			digitalWrite(YELLOW_LED, HIGH);  // Gelb EIN → Log-Datei Fehler
			break;
		case STATUS_LOG_WRITE:
			xTaskCreate(
			    [](void *param) {
				    while (true) {
					    digitalWrite(YELLOW_LED, HIGH);
					    vTaskDelay(pdMS_TO_TICKS(100));
					    digitalWrite(YELLOW_LED, LOW);
					    vTaskDelay(pdMS_TO_TICKS(100));
				    }
			    },
			    "LogWriteBlinkTask", 1024, NULL, 1, &blinkTaskHandle);
			break;
		case STATUS_NO_DEVICE_CONNECTED:
			digitalWrite(RED_LED, HIGH);  // Rot EIN → Kein Gerät mit AP verbunden
			break;
		case STATUS_DEVICE_CONNECTED:
			digitalWrite(RED_LED, LOW);  // Rot AUS → Gerät mit AP verbunden
			break;
	}
}

/**
 * Blinkt die gelbe LED, um anzuzeigen, dass Logs geschrieben werden.
 */
void logWriteBlink() {
	updateStatus(STATUS_LOG_WRITE);
}
