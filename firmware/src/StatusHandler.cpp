/**
 * @file StatusHandler.cpp
 * @brief Modul zur Verwaltung und Anzeige von System-Status über LED-Blinkmuster.
 *
 * Dieses Modul verwaltet mithilfe eines statischen Arrays und FreeRTOS-Tasks
 * verschiedene System-Status (z. B. INITIALIZING, READY, NO_SD_CARD, etc.) und zeigt
 * für jeden Status ein individuelles LED-Muster an. Ist kein Fehlerstatus aktiv,
 * wird automatisch SYSTEM_READY gesetzt und die grüne LED leuchtet dauerhaft.
 *
 * @author Simon Macel Linden
 * @since 1.0.0
 */

#include "StatusHandler.h"

#include <Arduino.h>

#include "LLog.h"

#define MAX_STATUSES 16

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
static TaskHandle_t apStationMonitorTaskHandle = NULL;

int statusPriority(SystemStatus status) {
	switch (status) {
		// 1) Kritischer Fehler: Rote LED dauerhaft
		case FS_NOT_AVAILABLE:
			return 100;  // höchste Prio

		// 2) Kritischer Fehler (Rot blinkend)
		case LOG_NO_DIR:
		case WEBSERVER_NO_HTML_DIR:
			return 90;

		// 3) Warning (Gelb dauerhaft)
		case WIFI_AP_NO_DEVICE:
			return 80;

		// 4) Warning (Gelb blinkend)
		case LOG_FILE_ERROR:
		case WIFI_STA_NOT_AVAILABLE:
		case WIFI_AP_NOT_AVAILABLE:
		case SERIAL_NOT_CONNECTED:
			return 70;

		// 5) Alles andere (Grün, etc.)
		case SERIAL_CONNECTED:
		case SYSTEM_INITIALIZING:
		case LOG_WRITE:
		case SERIAL_SEND:
			return 10;  // niedrige Prio

		default:
			return 1;  // Falls unbekannt
	}
}

void blinkedLED(int ledPin, int counter, int delayMs) {
	for (int i = 0; i < counter; i++) {
		digitalWrite(ledPin, HIGH);
		vTaskDelay(pdMS_TO_TICKS(delayMs));
		digitalWrite(ledPin, LOW);
		vTaskDelay(pdMS_TO_TICKS(delayMs));
	}
}

/********************************************************************/
/**
 * @brief Schaltet in den READY-Zustand: Grüne LED an, rote und gelbe LED aus,
 *        und führt eine Verzögerung aus.
 *
 * @param delayMs Die Dauer (in Millisekunden), für die die grüne LED leuchten soll.
 */
static void displayReadyState() {
	digitalWrite(RED_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);
	digitalWrite(GREEN_LED, LOW);
}

/**
 * @brief Prüft, ob ein bestimmter Systemstatus aktiv ist.
 *
 * Diese Funktion überprüft, ob der angegebene Status im internen Status-Array vorhanden ist.
 *
 * @param status Der zu prüfende Systemstatus.
 * @return true, falls der Status aktiv ist; andernfalls false.
 */
bool isStatusActive(SystemStatus status) {
	bool found = false;
	portENTER_CRITICAL(&g_statusMux);
	for (size_t i = 0; i < g_statusCount; i++) {
		if (g_statusArray[i] == status) {
			found = true;
			break;
		}
	}
	portEXIT_CRITICAL(&g_statusMux);
	return found;
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

	const int delaySlow = 500;    // 500ms (langsames Blinken)
	const int delayFast = 100;    // 100ms (schnelles Blinken)
	const int delayPause = 1500;  // 1,5s Pause zwischen den Mustern

	switch (status) {
		// 1) Kritischer Fehler: Rote LED dauerhaft
		case FS_NOT_AVAILABLE: {
			// Rote LED dauerhaft an
			digitalWrite(RED_LED, HIGH);
			break;
		}
		// 2) Kritischer Fehler (Rot blinkend)
		case LOG_NO_DIR: {
			// Rote LED blinkt 2× langsam
			blinkedLED(RED_LED, 2, delaySlow);
			break;
		}
		case WEBSERVER_NO_HTML_DIR: {
			blinkedLED(RED_LED, 3, delaySlow);
			break;
		}
		// 3) Warning (Gelb dauerhaft)
		case WIFI_AP_NO_DEVICE: {
			// Gelbe LED dauerhaft an
			digitalWrite(YELLOW_LED, HIGH);
			break;
		}
		// 4) Warning (Gelb blinkend)
		case SERIAL_NOT_CONNECTED: {
			blinkedLED(YELLOW_LED, 1, delaySlow);
			break;
		}
		case LOG_FILE_ERROR: {
			blinkedLED(YELLOW_LED, 2, delaySlow);
			break;
		}
		case WIFI_STA_NOT_AVAILABLE: {
			blinkedLED(YELLOW_LED, 3, delaySlow);
			break;
		}
		case WIFI_AP_NOT_AVAILABLE: {
			blinkedLED(YELLOW_LED, 4, delaySlow);
			break;
		}
		// 5) Systemstatus (Ready, etc.)
		case SERIAL_CONNECTED: {
			digitalWrite(GREEN_LED, HIGH);
			break;
		}
		case SYSTEM_INITIALIZING: {
			blinkedLED(GREEN_LED, 1, delaySlow);
			break;
		}
		case LOG_WRITE: {
			blinkedLED(GREEN_LED, 2, delaySlow);
			break;
		}
		case SERIAL_SEND: {
			blinkedLED(GREEN_LED, 3, delaySlow);
			break;
		}

		case SYSTEM_READY: {
			break;
		}
		case WIFI_AP_DEVICE_AVAILABLE: {
			digitalWrite(YELLOW_LED, LOW);
			break;
		}

		// DEFAULT: Kein passender Status
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
		// 1) Kopie der aktiven Status anlegen
		SystemStatus localArray[MAX_STATUSES];
		size_t localCount = 0;

		portENTER_CRITICAL(&g_statusMux);
		localCount = g_statusCount;
		if (localCount > MAX_STATUSES) {
			localCount = MAX_STATUSES;
		}
		for (size_t i = 0; i < localCount; i++) {
			localArray[i] = g_statusArray[i];
		}
		portEXIT_CRITICAL(&g_statusMux);

		// 2) Falls kein Status aktiv -> System ready
		if (localCount == 0) {
			// -> LED aus oder Grün an, je nach Definition:
			displayReadyState();
			// Kurze Pause und dann Schleife erneut
			vTaskDelay(pdMS_TO_TICKS(2000));
			continue;
		}

		// 3) Ermittle den Status mit der höchsten Priorität
		SystemStatus highestStatus = localArray[0];
		int highestPrio = statusPriority(highestStatus);

		for (size_t i = 1; i < localCount; i++) {
			int prio = statusPriority(localArray[i]);
			if (prio > highestPrio) {
				highestPrio = prio;
				highestStatus = localArray[i];
			}
		}

		// 4) Blinkmuster nur für diesen einen Status ausführen
		doBlinkPattern(highestStatus);

		// Danach loop erneut, um zu prüfen, ob sich was geändert hat
	}
}

/**
 * @brief Task-Funktion zur Überwachung der mit dem Access Point verbundenen Stationen.
 *
 * Diese Funktion überwacht kontinuierlich, ob Geräte (Stationen) mit dem Access Point verbunden sind.
 * Bei längerer Abwesenheit von Geräten wird der Status STATUS_NO_WIFI_DEVICE gesetzt.
 * Sobald Geräte wieder verbunden sind, wird dieser Fehlerstatus entfernt.
 * Zudem werden neu verbundene Geräte (MAC-Adressen) erkannt und geloggt.
 *
 * @param param Unbenutzter Parameter.
 */
static void apStationMonitorTask(void *param) {
	const int threshold = 2;  // z.B. 2 Zyklen à 2000ms = 4 Sekunden Debounce
	int consecutiveNoDevice = 0;
	int consecutiveDevicePresent = 0;

	// Liste der bisher bekannten MAC-Adressen
	std::vector<String> connectedMACs;

	for (;;) {
		wifi_sta_list_t wifi_sta_list;
		// Hole die Liste der verbundenen Stationen
		esp_wifi_ap_get_sta_list(&wifi_sta_list);
		int currentCount = wifi_sta_list.num;

		if (currentCount == 0) {
			consecutiveNoDevice++;
			consecutiveDevicePresent = 0;
			if (consecutiveNoDevice >= threshold) {
				// Kein Gerät verbunden: Status setzen, falls noch nicht vorhanden
				addStatus(WIFI_AP_NO_DEVICE);
				// Falls zuvor Geräte bekannt waren, dann leere die Liste und logge einmalig
				if (!connectedMACs.empty()) {
					connectedMACs.clear();
					logger.log({"system", "info", "wifi"}, "Kein Gerät mit dem AP verbunden.");
				}
			}
		} else {
			consecutiveDevicePresent++;
			consecutiveNoDevice = 0;
			if (consecutiveDevicePresent >= threshold) {
				// Mindestens ein Gerät verbunden: Fehlerstatus entfernen
				removeStatus(WIFI_AP_NO_DEVICE);

				// Erstelle eine Liste der aktuell verbundenen MAC-Adressen
				std::vector<String> newMACs;
				for (int i = 0; i < currentCount; i++) {
					uint8_t *mac = wifi_sta_list.sta[i].mac;
					char macStr[18];
					snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
					newMACs.push_back(String(macStr));
				}
				// Prüfe, ob es neue Geräte gibt (MACs, die in der alten Liste nicht vorhanden sind)
				for (const auto &mac : newMACs) {
					bool found = false;
					for (const auto &oldMac : connectedMACs) {
						if (mac == oldMac) {
							found = true;
							break;
						}
					}
					if (!found) {
						// Neues Gerät gefunden – logge dessen MAC-Adresse
						logger.log({"system", "info", "wifi"}, "Gerät mit dem AP verbunden. MAC: " + mac);
					}
				}
				// Aktualisiere die Liste der bekannten Geräte
				connectedMACs = newMACs;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

/**
 * @brief Initialisiert die LED-Pins und startet den Status-Task.
 *
 * Diese Funktion konfiguriert die verwendeten LED-Pins als Output, setzt sie initial auf LOW
 * und fügt optional den Initialisierungsstatus hinzu. Anschließend wird der FreeRTOS Task gestartet,
 * der die aktiven Status zyklisch abarbeitet.
 */
void startStatusSystem() {
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);

	digitalWrite(RED_LED, LOW);
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	// Falls gewünscht: Initialstatus "SYSTEM_INITIALIZING" direkt hinzufügen
	addStatus(SYSTEM_INITIALIZING);

	if (statusTaskHandle == NULL) {
		xTaskCreatePinnedToCore(statusTask, "StatusTask", 4096, NULL, 1, &statusTaskHandle, 1);
	}

	if (apStationMonitorTaskHandle == NULL) {
		xTaskCreatePinnedToCore(apStationMonitorTask, "APStationMonitor", 4096, NULL, 1, &apStationMonitorTaskHandle, 1);
	}
}

/**
 * @brief Fügt einen Status zur Liste hinzu, falls dieser noch nicht vorhanden ist.
 *
 * Wird ein Fehlerstatus (ungleich SYSTEM_READY) hinzugefügt, wird eventuell vorhandener
 * SYSTEM_READY entfernt, um anzuzeigen, dass ein Fehler vorliegt. Anschließend wird der Status
 * dem internen Array hinzugefügt, sofern noch Platz vorhanden ist.
 *
 * @param status Systemstatus.
 */
void addStatus(SystemStatus status) {
	portENTER_CRITICAL(&g_statusMux);
	bool wasInserted = false;
	bool found = false;

	// Entferne SYSTEM_READY, wenn ein Fehlerstatus hinzugefügt wird
	if (status != SYSTEM_READY) {
		for (size_t i = 0; i < g_statusCount; i++) {
			if (g_statusArray[i] == SYSTEM_READY) {
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
 * sind, wird automatisch SYSTEM_READY gesetzt.
 *
 * @param status Systemstatus.
 */
void removeStatus(SystemStatus status) {
	portENTER_CRITICAL(&g_statusMux);
	for (size_t i = 0; i < g_statusCount; i++) {
		if (g_statusArray[i] == status) {
			for (size_t j = i; j < (g_statusCount - 1); j++) {
				g_statusArray[j] = g_statusArray[j + 1];
			}
			g_statusCount--;
			break;
		}
	}
	portEXIT_CRITICAL(&g_statusMux);
}
