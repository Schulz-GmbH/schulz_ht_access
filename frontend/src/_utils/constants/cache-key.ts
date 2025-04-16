const SYSTEM_NAME = 'schulz-gmbh'

export class CacheKey {
	static readonly TOKEN = `${SYSTEM_NAME}-token-key`
	static readonly CONFIG_LAYOUT = `${SYSTEM_NAME}-config-layout-key`
	static readonly NAVBAR_STATUS = `${SYSTEM_NAME}-navbar-status-key`
	static readonly ACTIVE_THEME_NAME = `${SYSTEM_NAME}-active-theme-name-key`
	static readonly VISITED_VIEWS = `${SYSTEM_NAME}-visited-views-key`
	static readonly CACHED_VIEWS = `${SYSTEM_NAME}-cached-views-key`
}
