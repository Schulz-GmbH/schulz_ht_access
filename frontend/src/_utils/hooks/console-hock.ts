/**
 * @file console-hook.ts
 * @brief Hook für die Konsolen-Methoden mit Timestamp-Logging und IndexedDB-Persistenz.
 *
 * Detaillierte Beschreibung:
 * Dieses Modul überschreibt die Standard-Console-Methoden (log, info, warn, error, debug),
 * um jede Ausgabe mit einem Zeitstempel und Log-Level-Tag anzureichern und die Log-Nachrichten
 * zusätzlich in der IndexedDB zu speichern (mittels saveLogToIndexedDB). Eine dynamische
 * Aktivierung erfolgt basierend auf dem Environment-Mode oder einem System-Store-Flag.
 *
 * @author Simon Marcel Linden
 * @version 1.0.0
 * @since 1.0.0
 */

import { saveLogToIndexedDB } from "@/_utils/log"; // Pfad anpassen

/**
 * @brief Speichert die ursprünglichen Konsolen-Methoden vor dem Hook.
 */
let originalConsole: Partial<Console> | null = null;

/**
 * @brief Gibt den aktuellen Zeitstempel im Format YYYY-MM-DD hh:mm:ss zurück.
 *
 * @return {string} Formatierter Zeitstempel.
 */
function getTimestamp(): string {
	const now = new Date();
	return now.toISOString().replace("T", " ").substring(0, 19);
}

/**
 * @brief Prüft, ob Logging erlaubt ist.
 *
 * Logging ist aktiviert, wenn der Modus 'development' ist oder
 * das Flag `systemStore.logging` gesetzt ist.
 *
 * @return {boolean} true, wenn Logging zulässig ist.
 */
function isLoggingEnabled(): boolean {
	try {
		const systemStore = require("@/store/system/index.store").useSystemStore();
		return import.meta.env.MODE === "development" || systemStore.logging;
	} catch {
		return import.meta.env.MODE === "development";
	}
}

/**
 * @brief Erstellt einen Wrapper für eine Konsolen-Methode mit Prefix und Persistenz.
 *
 * @param[in] method  Ursprüngliche Konsolen-Methode (z.B. console.log).
 * @param[in] level   Log-Level-Typ (z.B. "LOG", "INFO", "WARN", "ERROR", "DEBUG").
 * @return {Function} Wrapped-Funktion, die Timestamp-Logging und Speicherung übernimmt.
 */
function createWrapper(method: (...args: any[]) => void, level: string) {
	return async (...args: any[]) => {
		if (!isLoggingEnabled()) return;

		const prefix = `[${getTimestamp()}][${level}]`;
		method.call(console, prefix, ...args);

		const stringified = args
			.map((arg) => {
				if (typeof arg === "object") {
					try {
						return JSON.stringify(arg);
					} catch {
						return "[Object]";
					}
				}
				return String(arg);
			})
			.join(" ");

		await saveLogToIndexedDB(`${prefix} ${stringified}`);
	};
}

/**
 * @brief Überschreibt die wichtigsten console-Methoden mit Timestamp-Logging und Persistenz.
 *
 * Die Methoden log, info, warn, error und debug werden gemappt
 * und durch Wrappers substituiert, die Timestamp-Logging und
 * IndexedDB-Persistenz implementieren.
 */
export function hookConsole(): void {
	if (originalConsole) return; // bereits gehookt

	originalConsole = {
		log: console.log,
		info: console.info,
		warn: console.warn,
		error: console.error,
		debug: console.debug,
	};

	console.log = createWrapper(console.log, "LOG");
	console.info = createWrapper(console.info, "INFO");
	console.warn = createWrapper(console.warn, "WARN");
	console.error = createWrapper(console.error, "ERROR");
	console.debug = createWrapper(console.debug, "DEBUG");
}

/**
 * @brief Stellt die ursprünglichen console-Methoden wieder her.
 *
 * Setzt die Methoden log, info, warn, error und debug auf die zuvor
 * gespeicherten Originalfunktionen zurück.
 */
export function unhookConsole(): void {
	if (!originalConsole) return;

	console.log = originalConsole.log!;
	console.info = originalConsole.info!;
	console.warn = originalConsole.warn!;
	console.error = originalConsole.error!;
	console.debug = originalConsole.debug!;

	originalConsole = null;
}
