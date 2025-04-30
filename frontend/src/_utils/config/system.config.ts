import { getSystemConfig } from "@/_utils/cache/system.storage.cache";
import { connect } from "http2";

/**
 * Definition der System-Konfigurationsoptionen
 */
export interface SystemConfig {
	version: { loading: boolean; value: string };
	logging: { loading: boolean; state: boolean };
	wlan: { loading: boolean; status: boolean; connected: boolean };
	serial: { loading: boolean; available: boolean; baudRate: number; connected: boolean };
}

/**
 * Standardkonfiguration für das System
 */
const DEFAULT_SETTINGS_CONFIG: Readonly<SystemConfig> = {
	version: { loading: false, value: "1.0.0" },
	logging: { loading: false, state: false },
	wlan: { loading: false, status: false, connected: false },
	serial: { loading: false, available: false, baudRate: 9600, connected: false },
};

const storageData: Partial<SystemConfig> = getSystemConfig() ?? {};
const { serial: _ignoreSerial, ...persisted } = storageData;

/**
 * Aktuelle System-Konfiguration (Zusammenführung aus Standard- und gespeicherten Einstellungen)
 */
export const systemConfig: SystemConfig = {
	...DEFAULT_SETTINGS_CONFIG,
	...persisted,
};
