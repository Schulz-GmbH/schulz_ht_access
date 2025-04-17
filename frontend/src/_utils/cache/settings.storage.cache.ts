import type { SettingsConfig } from "@/_utils/config/settings.config";
import { CacheKey } from "@/_utils/constants/cache-key";

/**
 * Holt die gespeicherte System-Konfiguration aus dem `localStorage`.
 * @returns {SystemConfig | null} Die gespeicherte Konfiguration oder `null`, falls nicht vorhanden.
 */
export function getSystemConfig(): SettingsConfig | null {
	const json = localStorage.getItem(CacheKey.SETTINGS_CONFIG);
	try {
		return json ? (JSON.parse(json) as SettingsConfig) : null;
	} catch (error) {
		console.error("Fehler beim Parsen von SystemConfig:", error);
		return null;
	}
}

/**
 * Speichert die System-Konfiguration im `localStorage`.
 * @param {SettingsConfig} config - Die zu speichernde System-Konfiguration.
 */
export function setSystemConfig(config: SettingsConfig): void {
	localStorage.setItem(CacheKey.SETTINGS_CONFIG, JSON.stringify(config));
}

/**
 * Entfernt die gespeicherte System-Konfiguration aus dem `localStorage`.
 */
export function removeSystemConfig(): void {
	localStorage.removeItem(CacheKey.SETTINGS_CONFIG);
}
