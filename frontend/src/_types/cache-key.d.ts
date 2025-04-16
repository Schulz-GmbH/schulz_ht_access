declare module '@/constants/cache-key' {
	/** Definiert System-spezifische Cache-Schlüssel */
	export class CacheKey {
		static readonly TOKEN: string
		static readonly CONFIG_LAYOUT: string
		static readonly NAVBAR_STATUS: string
		static readonly ACTIVE_THEME_NAME: string
		static readonly VISITED_VIEWS: string
		static readonly CACHED_VIEWS: string
	}
}
