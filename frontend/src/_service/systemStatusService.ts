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

import { SocketService } from "@/_service/socket";
import { useSettingsStore } from "@/store/settings/index.store";

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
 * @param[in] settingsStore Instanz des Settings-Stores (useSettingsStore()).
 * @return {Promise<void>} Promise, das aufgelöst wird, wenn der Vorgang abgeschlossen ist.
 */
async function fetchWifiStatus(settingsStore: ReturnType<typeof useSettingsStore>): Promise<void> {
	settingsStore.wlan.loading = true;
	try {
		const listener = new Promise<void>((resolve) => {
			const handler = (data: { status: string; details: string }) => {
				if (data.status === "success") {
					settingsStore.wlan.status = data.details !== "disabled";
					settingsStore.wlan.connected = data.details === "connected";
				}
				SocketService.removeListener("system", "wifi", handler);
				resolve();
			};
			SocketService.onMessage("system", "wifi", handler);
		});

		await SocketService.sendMessage({
			type: "system",
			command: "wifi",
			key: "status",
		});

		await Promise.race([listener, createTimeout(3000, "Timeout beim Abrufen des WLAN-Status")]);
	} catch (err) {
		console.warn("WLAN-Status konnte nicht abgerufen werden:", err);
		settingsStore.wlan.status = false;
		settingsStore.wlan.connected = false;
	} finally {
		settingsStore.wlan.loading = false;
	}
}

/**
 * @brief Fragt den Log-Status vom Server ab und speichert ihn im Settings-Store.
 *
 * Registriert einen temporären Listener für das Event "log","debug", sendet den Befehl
 * und wartet bis zum Timeout oder bis eine Antwort eintrifft. Aktualisiert anschließend
 * die Felder `logging.state` und `logging.loading`.
 *
 * @param[in] settingsStore Instanz des Settings-Stores (useSettingsStore()).
 * @return {Promise<void>} Promise, das aufgelöst wird, wenn der Vorgang abgeschlossen ist.
 */
async function fetchLogStatus(settingsStore: ReturnType<typeof useSettingsStore>): Promise<void> {
	settingsStore.logging.loading = true;
	try {
		const listener = new Promise<void>((resolve) => {
			const handler = (data: { status: string; details: string }) => {
				settingsStore.logging.state = data.status === "success" && data.details === "true";
				SocketService.removeListener("log", "debug", handler);
				resolve();
			};
			SocketService.onMessage("log", "debug", handler);
		});

		await SocketService.sendMessage({
			type: "log",
			command: "debug",
			key: "status",
		});

		await Promise.race([listener, createTimeout(3000, "Timeout beim Abrufen des Log-Status")]);
	} catch (err) {
		console.warn("Log-Status konnte nicht abgerufen werden:", err);
		settingsStore.logging.state = false;
	} finally {
		settingsStore.logging.loading = false;
	}
}

/**
 * @brief Fragt die aktuelle App-Version vom Server ab und speichert sie im Settings-Store.
 *
 * Registriert einen temporären Listener für das Event "system","version", sendet den Befehl
 * und wartet bis zum Timeout oder bis eine Antwort eintrifft. Aktualisiert anschließend
 * das Feld `version.value` und `version.loading`.
 *
 * @param[in] settingsStore Instanz des Settings-Stores (useSettingsStore()).
 * @return {Promise<void>} Promise, das aufgelöst wird, wenn der Vorgang abgeschlossen ist.
 */
async function fetchVersion(settingsStore: ReturnType<typeof useSettingsStore>): Promise<void> {
	settingsStore.version.loading = true;
	try {
		const listener = new Promise<void>((resolve) => {
			const handler = (data: { status: string; details: string }) => {
				if (data.status === "success") {
					settingsStore.version.value = data.details;
				}
				SocketService.removeListener("system", "version", handler);
				resolve();
			};
			SocketService.onMessage("system", "version", handler);
		});

		await SocketService.sendMessage({
			type: "system",
			command: "version",
			key: "get",
		});

		await Promise.race([listener, createTimeout(3000, "Timeout beim Abrufen der App-Version")]);
	} catch (err) {
		console.warn("App-Version konnte nicht abgerufen werden:", err);
		settingsStore.version.value = "unbekannt";
	} finally {
		settingsStore.version.loading = false;
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

	const settingsStore = useSettingsStore();
	await Promise.allSettled([fetchWifiStatus(settingsStore), fetchLogStatus(settingsStore), fetchVersion(settingsStore)]);
}
