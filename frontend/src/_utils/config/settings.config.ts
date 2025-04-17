import { getSystemConfig } from "@/_utils/cache/settings.storage.cache";

/**
 * Definition der System-Konfigurationsoptionen
 */
export interface SettingsConfig {
	version: string;
	logging: boolean;
}

/**
 * Standardkonfiguration für das System
 */
const DEFAULT_SETTINGS_CONFIG: Readonly<SettingsConfig> = {
	version: "1.0.0",
	logging: false,
};

/**
 * Aktuelle System-Konfiguration (Zusammenführung aus Standard- und gespeicherten Einstellungen)
 */
export const systemConfig: SettingsConfig = {
	...DEFAULT_SETTINGS_CONFIG,
	...getSystemConfig(),
};
