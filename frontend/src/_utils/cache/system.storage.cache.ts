import type { SystemConfig } from "@/_utils/config/system.config";
import { CacheKey } from "@/_utils/constants/cache-key";

/**
 * Holt die gespeicherte System-Konfiguration aus dem `localStorage`.
 * @returns {SystemConfig | null} Die gespeicherte Konfiguration oder `null`, falls nicht vorhanden.
 */
export function getSystemConfig(): SystemConfig | null {
	const json = localStorage.getItem(CacheKey.SYSTEM_CONFIG);
	try {
		return json ? (JSON.parse(json) as SystemConfig) : null;
	} catch (error) {
		console.error("Fehler beim Parsen von SystemConfig:", error);
		return null;
	}
}

/**
 * Speichert die System-Konfiguration im `localStorage`.
 * @param {SystemConfig} config - Die zu speichernde System-Konfiguration.
 */
export function setSystemConfig(config: SystemConfig): void {
	localStorage.setItem(CacheKey.SYSTEM_CONFIG, JSON.stringify(config));
}

/**
 * Entfernt die gespeicherte System-Konfiguration aus dem `localStorage`.
 */
export function removeSystemConfig(): void {
	localStorage.removeItem(CacheKey.SYSTEM_CONFIG);
}
