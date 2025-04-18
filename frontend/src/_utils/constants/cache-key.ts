const SYSTEM_NAME = "schulz-gmbh";

export class CacheKey {
	static readonly LAYOUT_CONFIG = `${SYSTEM_NAME}-config-layout-key`;
	static readonly SETTINGS_CONFIG = `${SYSTEM_NAME}-config-settings-key`;
	static readonly NAVBAR_STATUS = `${SYSTEM_NAME}-navbar-status-key`;
	static readonly ACTIVE_THEME = `${SYSTEM_NAME}-active-theme-name-key`;
}
