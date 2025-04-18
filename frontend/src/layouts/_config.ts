import { LayoutModeEnum } from "@/_utils/constants/app-key";
import { getLayoutsConfig } from "@/_utils/cache/local-storage";

/**
 * Definition der Layout-Konfigurationsoptionen
 */
export interface LayoutsConfig {
	layout: LayoutModeEnum;
	showLogo: boolean;
	fixedHeader: boolean;
	showFooter: boolean;
	showNotify: boolean;
	showSearchMenu: boolean;
}

/**
 * Standardkonfiguration für das Layout
 */
const DEFAULT_CONFIG: Readonly<LayoutsConfig> = {
	layout: LayoutModeEnum.Top,
	fixedHeader: true,
	showFooter: true,
	showLogo: true,
	showNotify: true,
	showSearchMenu: true,
};

/**
 * Aktuelle Layout-Konfiguration (Zusammenführung aus Standard- und gespeicherten Einstellungen)
 */
export const layoutsConfig: LayoutsConfig = {
	...DEFAULT_CONFIG,
	...getLayoutsConfig(),
};
