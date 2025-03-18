/**
 * @file StatusHandler.cpp
 * @brief Modul zur Verwaltung und Anzeige von System-Status über LED-Blinkmuster.
 *
 * Dieses Modul verwaltet mithilfe eines statischen Arrays und FreeRTOS-Tasks
 * verschiedene System-Status (z. B. INITIALIZING, READY, NO_SD_CARD, etc.) und zeigt
 * für jeden Status ein individuelles LED-Muster an. Ist kein Fehlerstatus aktiv,
 * wird automatisch STATUS_READY gesetzt und die grüne LED leuchtet dauerhaft.
 *
 * @author Simon Macel Linden
 * @since 1.0.0
 */

#include "StatusHandler.h"

#include "LLog.h"

#define MAX_STATUSES 6

/*
   FreeRTOS Task verwenden, um alle aktiven Status nacheinander anzuzeigen.
   Jeder Status hat sein eigenes Blink-/Dauerlichtmuster für die zugehörige(n) LED(s).
   Alle aktive Status werden solange abgearbeitet,
   bis sie per removeStatus() entfernt wurden.
*/

// Statisches Array und Zähler, um die aktiven Status zu verwalten
static SystemStatus g_statusArray[MAX_STATUSES];
static size_t g_statusCount = 0;

// Mutex (Critical Section) zum Schutz der Zugriffe auf g_statusArray
static portMUX_TYPE g_statusMux = portMUX_INITIALIZER_UNLOCKED;

// Taskhandle für das LED-Blinken
static TaskHandle_t statusTaskHandle = NULL;

/**
 * @brief Schaltet in den READY-Zustand: Grüne LED an, rote und gelbe LED aus,
 *        und führt eine Verzögerung aus.
 *
 * @param delayMs Die Dauer (in Millisekunden), für die die grüne LED leuchten soll.
 */
static void displayReadyState(int delayMs) {
	digitalWrite(RED_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);
	digitalWrite(GREEN_LED, HIGH);
	vTaskDelay(pdMS_TO_TICKS(delayMs));
}

/**
 * @brief Führt das Blink-/Leuchtmuster für einen einzelnen Status aus.
 *
 * Diese Funktion schaltet zunächst alle LEDs aus und führt dann
 * ein blink-/dauerhaftes Leuchtmuster anhand des übergebenen Status aus.
 * Nach dem Muster folgt eine Pause, um die Anzeige einzelner Status voneinander zu trennen.
 *
 * @param status Systemstatus.
 */
static void doBlinkPattern(SystemStatus status) {
	// Alle LEDs zunächst ausschalten
	digitalWrite(RED_LED, LOW);
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	int delaySlow = 500;    // Standardverzögerung
	int delayFast = 100;    // Kurze Verzögerung
	int delayPause = 1500;  // Standardverzögerung

	switch (status) {
		case STATUS_INITIALIZING:
			// Grünes kurzes Blinken (1 Sekunde)
			digitalWrite(GREEN_LED, HIGH);
			vTaskDelay(pdMS_TO_TICKS(delayFast));
			digitalWrite(GREEN_LED, LOW);
			vTaskDelay(pdMS_TO_TICKS(delayFast));
			break;

		case STATUS_READY:
			displayReadyState(100);
			break;

		case STATUS_NO_SD_CARD:
			digitalWrite(RED_LED, HIGH);
			break;

		case STATUS_NO_HTML_DIR:
			for (int i = 0; i < 3; i++) {
				digitalWrite(RED_LED, HIGH);
				vTaskDelay(pdMS_TO_TICKS(delaySlow));
				digitalWrite(RED_LED, LOW);
				vTaskDelay(pdMS_TO_TICKS(delaySlow));
			}
			break;

		case STATUS_NO_LOGS_DIR:
			for (int i = 0; i < 2; i++) {
				digitalWrite(RED_LED, HIGH);
				vTaskDelay(pdMS_TO_TICKS(delaySlow));
				digitalWrite(RED_LED, LOW);
				vTaskDelay(pdMS_TO_TICKS(delaySlow));
			}
			break;

		default:
			vTaskDelay(pdMS_TO_TICKS(2000));
			break;
	}

	// Kleine Pause zwischen den einzelnen Status-Mustern
	vTaskDelay(pdMS_TO_TICKS(delayPause));
}

/**
 * @brief Task-Funktion, die alle aktiven Status zyklisch abarbeitet.
 *
 * Diese Funktion erstellt in einer Critical Section eine Kopie des Status-Arrays
 * und iteriert dann außerhalb der Critical Section über die Status, um deren
 * jeweilige LED-Muster mit doBlinkPattern() anzuzeigen.
 *
 * Falls kein Status aktiv ist (localCount == 0), wird displayReadyState() aufgerufen,
 * sodass die grüne LED dauerhaft leuchtet.
 *
 * @param param Unbenutzt.
 */
static void statusTask(void *param) {
	for (;;) {
		SystemStatus localArray[MAX_STATUSES];
		size_t localCount = 0;

		portENTER_CRITICAL(&g_statusMux);
		localCount = g_statusCount;
		// Grenzen checken, um Sicherheit zu haben
		if (localCount > MAX_STATUSES) {
			localCount = MAX_STATUSES;
		}
		for (size_t i = 0; i < localCount; i++) {
			localArray[i] = g_statusArray[i];
		}
		portEXIT_CRITICAL(&g_statusMux);

		if (localCount == 0) {
			// Wenn kein Status aktiv ist -> LED grün an
			displayReadyState(2000);
		} else {
			for (size_t i = 0; i < localCount; i++) {
				doBlinkPattern(localArray[i]);
			}
		}
	}
}

/**
 * @brief Initialisiert die LED-Pins und startet den Status-Task.
 *
 * Diese Funktion konfiguriert die verwendeten LED-Pins als Output, setzt sie initial auf LOW
 * und fügt optional den Initialisierungsstatus hinzu. Anschließend wird der FreeRTOS Task gestartet,
 * der die aktiven Status zyklisch abarbeitet.
 */
void setupStatusSystem() {
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);

	digitalWrite(RED_LED, LOW);
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	// Falls gewünscht: Initialstatus "STATUS_INITIALIZING" direkt hinzufügen
	addStatus(STATUS_INITIALIZING);

	if (statusTaskHandle == NULL) {
		xTaskCreate(statusTask, "StatusTask", 4096, NULL, 1, &statusTaskHandle);
	}
}

/**
 * @brief Fügt einen Status zur Liste hinzu, falls dieser noch nicht vorhanden ist.
 *
 * Wird ein Fehlerstatus (ungleich STATUS_READY) hinzugefügt, wird eventuell vorhandener
 * STATUS_READY entfernt, um anzuzeigen, dass ein Fehler vorliegt. Anschließend wird der Status
 * dem internen Array hinzugefügt, sofern noch Platz vorhanden ist.
 *
 * @param status Systemstatus.
 */
void addStatus(SystemStatus status) {
	portENTER_CRITICAL(&g_statusMux);
	bool wasInserted = false;
	bool found = false;

	// Entferne STATUS_READY, wenn ein Fehlerstatus hinzugefügt wird
	if (status != STATUS_READY) {
		for (size_t i = 0; i < g_statusCount; i++) {
			if (g_statusArray[i] == STATUS_READY) {
				for (size_t j = i; j < (g_statusCount - 1); j++) {
					g_statusArray[j] = g_statusArray[j + 1];
				}
				g_statusCount--;
				break;
			}
		}
	}

	// Überprüfen, ob der Status bereits vorhanden ist
	for (size_t i = 0; i < g_statusCount; i++) {
		if (g_statusArray[i] == status) {
			found = true;
			break;
		}
	}

	// Nur hinzufügen, wenn nicht vorhanden und noch Platz ist
	if (!found && g_statusCount < MAX_STATUSES) {
		g_statusArray[g_statusCount++] = status;
		wasInserted = true;
	}

	portEXIT_CRITICAL(&g_statusMux);
}

/**
 * @brief Entfernt einen Status aus der Liste.
 *
 * Diese Funktion sucht den übergebenen Status im internen Array und entfernt ihn,
 * indem sie die folgenden Elemente nachrückt. Falls danach keine Fehlerstatus mehr vorhanden
 * sind, wird automatisch STATUS_READY gesetzt.
 *
 * @param status Systemstatus.
 */
void removeStatus(SystemStatus status) {
	portENTER_CRITICAL(&g_statusMux);

	bool removed = false;
	for (size_t i = 0; i < g_statusCount; i++) {
		if (g_statusArray[i] == status) {
			for (size_t j = i; j < (g_statusCount - 1); j++) {
				g_statusArray[j] = g_statusArray[j + 1];
			}
			g_statusCount--;
			removed = true;
			break;
		}
	}

	portEXIT_CRITICAL(&g_statusMux);

	// Falls keine Fehlerstatus mehr vorhanden sind, setze STATUS_READY
	if (g_statusCount == 0) {
		g_statusArray[g_statusCount++] = STATUS_READY;
	}
}
