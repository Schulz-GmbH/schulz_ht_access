/**
 * @file systemStatusService.ts
 * @brief Service zum Abrufen systemrelevanter Statuswerte via WebSocket.
 *
 * Detaillierte Beschreibung:
 * Dieses Modul stellt Funktionen bereit, um den WLAN-Status, den Log-Status und die App-Version
 * von einem Backend über WebSocket abzufragen und im Settings-Store zu speichern.
 * Ein Timeout schützt vor zu langem Warten bei fehlenden Antworten.
 *
 * @author Simon Marcel Linden
 * @version 1.0.0
 * @since 1.0.0
 */

const timeoutTime = 5000; // Timeout in Millisekunden

import { SocketService } from "@/_service/socket";
import { useSystemStore } from "@/store/system/index.store";

/**
 * @brief Erzeugt ein Timeout-Promise, das nach `ms` Millisekunden mit einem Fehler abbricht.
 *
 * @param[in] ms Zeit in Millisekunden bis zum Timeout.
 * @param[in] message Fehlermeldung, die beim Timeout geworfen wird.
 * @return {Promise<never>} Promise, das nach Ablauf der Zeit mit Error(message) rejected.
 */
function createTimeout(ms: number, message: string): Promise<never> {
	return new Promise((_, reject) => setTimeout(() => reject(new Error(message)), ms));
}

/**
 * @brief Fragt den WLAN-Status vom Server ab und speichert ihn im Settings-Store.
 *
 * Registriert einen temporären Listener für das Event "system","wifi", sendet den Befehl
 * und wartet bis zum Timeout oder bis eine Antwort eintrifft. Aktualisiert anschließend
 * die Felder `wlan.status`, `wlan.connected` und `wlan.loading`.
 *
 * @param[in] systemStore Instanz des Settings-Stores (useSystemStore()).
 * @return {Promise<void>} Promise, das aufgelöst wird, wenn der Vorgang abgeschlossen ist.
 */
async function fetchWifiStatus(systemStore: ReturnType<typeof useSystemStore>): Promise<void> {
	systemStore.wlan.loading = true;
	try {
		const listener = new Promise<void>(async (resolve) => {
			const handler = (data: { status: string; details: string }) => {
				if (data.status === "success") {
					systemStore.wlan.status = data.details !== "disabled";
					systemStore.wlan.connected = data.details === "connected";
				}
				SocketService.removeListener("system", "wifi", handler);
				resolve();
			};
			await SocketService.onMessage("system", "wifi", handler);
		});

		await SocketService.sendMessage({
			type: "system",
			command: "wifi",
			key: "status",
		});

		await Promise.race([listener, createTimeout(timeoutTime, "Timeout beim Abrufen des WLAN-Status")]);
	} catch (err) {
		console.warn("WLAN-Status konnte nicht abgerufen werden:", err);
		systemStore.wlan.status = false;
		systemStore.wlan.connected = false;
	} finally {
		systemStore.wlan.loading = false;
	}
}

/**
 * @brief Fragt den Log-Status vom Server ab und speichert ihn im Settings-Store.
 *
 * Registriert einen temporären Listener für das Event "log","debug", sendet den Befehl
 * und wartet bis zum Timeout oder bis eine Antwort eintrifft. Aktualisiert anschließend
 * die Felder `logging.state` und `logging.loading`.
 *
 * @param[in] systemStore Instanz des Settings-Stores (useSystemStore()).
 * @return {Promise<void>} Promise, das aufgelöst wird, wenn der Vorgang abgeschlossen ist.
 */
async function fetchLogStatus(systemStore: ReturnType<typeof useSystemStore>): Promise<void> {
	systemStore.logging.loading = true;
	try {
		const listener = new Promise<void>(async (resolve) => {
			const handler = (data: { status: string; details: string }) => {
				systemStore.logging.state = data.status === "success" && data.details === "true";
				SocketService.removeListener("log", "debug", handler);
				resolve();
			};
			await SocketService.onMessage("log", "debug", handler); // <== entscheidend
		});

		await SocketService.sendMessage({
			type: "log",
			command: "debug",
			key: "status",
		});

		await Promise.race([listener, createTimeout(timeoutTime, "Timeout beim Abrufen des Log-Status")]);
	} catch (err) {
		console.warn("Log-Status konnte nicht abgerufen werden:", err);
		systemStore.logging.state = false;
	} finally {
		systemStore.logging.loading = false;
	}
}

/**
 * @brief Fragt die aktuelle App-Version vom Server ab und speichert sie im Settings-Store.
 *
 * Registriert einen temporären Listener für das Event "system","version", sendet den Befehl
 * und wartet bis zum Timeout oder bis eine Antwort eintrifft. Aktualisiert anschließend
 * das Feld `version.value` und `version.loading`.
 *
 * @param[in] systemStore Instanz des Settings-Stores (useSystemStore()).
 * @return {Promise<void>} Promise, das aufgelöst wird, wenn der Vorgang abgeschlossen ist.
 */
async function fetchVersion(systemStore: ReturnType<typeof useSystemStore>): Promise<void> {
	systemStore.version.loading = true;
	try {
		const listener = new Promise<void>(async (resolve) => {
			const handler = (data: { status: string; details: string }) => {
				if (data.status === "success") {
					systemStore.version.value = data.details;
				}
				SocketService.removeListener("system", "version", handler);
				resolve();
			};
			await SocketService.onMessage("system", "version", handler);
		});

		await SocketService.sendMessage({
			type: "system",
			command: "version",
			key: "get",
		});

		await Promise.race([listener, createTimeout(timeoutTime, "Timeout beim Abrufen der App-Version")]);
	} catch (err) {
		console.warn("App-Version konnte nicht abgerufen werden:", err);
		systemStore.version.value = "unbekannt";
	} finally {
		systemStore.version.loading = false;
	}
}

/**
 * @brief Fragt den aktuellen Status der Serial-Schnittstelle vom Server ab und speichert sie im System-Store.
 *
 * Registriert einen temporären Listener für das Event "serial","status",
 * und wartet bis zum Timeout oder bis eine Antwort eintrifft. Aktualisiert anschließend
 * das Feld `available` und `baudRate`.
 *
 * @param[in] systemStore Instanz des Settings-Stores (useSystemStore()).
 * @return {Promise<void>} Promise, das aufgelöst wird, wenn der Vorgang abgeschlossen ist.
 */
async function fetchSerial(systemStore: ReturnType<typeof useSystemStore>): Promise<void> {
	systemStore.serial.loading = true;
	try {
		const listener = new Promise<void>(async (resolve) => {
			const handler = (data: { details: { available: boolean; baudRate: number } }) => {
				console.log("Serial-Status erhalten:", data);
				systemStore.serial.available = data.details.available;
				systemStore.serial.baudRate = data.details.baudRate;
				// SocketService.removeListener("serial", "status", handler);
				resolve();
			};
			await SocketService.onMessage("serial", "status", handler);
		});

		await Promise.race([listener, createTimeout(timeoutTime, "Timeout beim Warten auf Serial-Status")]);
	} catch (err) {
		console.warn("Serial-Status konnte nicht empfangen werden:", err);
		systemStore.serial.available = false;
		systemStore.serial.baudRate = 0;
	} finally {
		systemStore.serial.loading = false;
	}
}

/**
 * @brief Initialisiert alle systemrelevanten Statuswerte im Settings-Store.
 *
 * Wenn die PWA offline ist, wird die Initialisierung abgebrochen.
 * Andernfalls werden parallel WLAN-Status, Log-Status und App-Version abgefragt.
 *
 * @return {Promise<void>} Promise, das aufgelöst wird, sobald alle Abfragen beendet sind.
 */
export async function systemStatusService(): Promise<void> {
	if (!navigator.onLine) {
		console.warn("PWA ist offline – Systemstatus kann nicht geladen werden.");
		return;
	}

	await SocketService.connect();

	const systemStore = useSystemStore();
	await Promise.allSettled([fetchWifiStatus(systemStore), fetchLogStatus(systemStore), fetchVersion(systemStore), fetchSerial(systemStore)]);
}
