// console-hook.ts

let originalConsole: Partial<Console> | null = null;

/**
 * Gibt den aktuellen Zeitstempel im Format YYYY-MM-DD hh:mm:ss zurück.
 */
function getTimestamp(): string {
	const now = new Date();
	return now.toISOString().replace("T", " ").substring(0, 19);
}

/**
 * Prüft, ob Logging erlaubt ist (basierend auf Umgebung und optionalem systemStore.logging).
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
 * Erstellt ein Wrapper für console-Methoden mit Prefix.
 */
function createWrapper(method: (...args: any[]) => void, level: string) {
	return (...args: any[]) => {
		if (!isLoggingEnabled()) return;
		method.call(console, `[${getTimestamp()}][${level}]`, ...args);
	};
}

/**
 * Überschreibt die wichtigsten console-Methoden (log, info, warn, error, debug)
 * mit Timestamp-Logging und dynamischer Aktivierung.
 */
export function hookConsole(): void {
	if (originalConsole) return; // schon gehooked

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
 * Stellt die ursprünglichen console-Methoden wieder her.
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
