import { getSystemConfig } from "@/_utils/cache/settings.storage.cache";
import { connect } from "http2";

/**
 * Definition der System-Konfigurationsoptionen
 */
export interface SettingsConfig {
	version: { loading: boolean; value: string };
	logging: { loading: boolean; state: boolean };
	wlan: {
		loading: boolean;
		status: boolean;
		connected: boolean;
	};
}

/**
 * Standardkonfiguration für das System
 */
const DEFAULT_SETTINGS_CONFIG: Readonly<SettingsConfig> = {
	version: { loading: false, value: "1.0.0" },
	logging: { loading: false, state: false },
	wlan: { loading: false, status: false, connected: false },
};

/**
 * Aktuelle System-Konfiguration (Zusammenführung aus Standard- und gespeicherten Einstellungen)
 */
export const systemConfig: SettingsConfig = {
	...DEFAULT_SETTINGS_CONFIG,
	...getSystemConfig(),
};
