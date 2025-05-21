// src/store/system/index.store.ts
import { defineStore } from 'pinia';
import { reactive, toRefs, watch } from 'vue';
import { constantRoutes, developmentRoutes } from '@/router';
import { systemConfig } from '@/_utils/config/system';
import { setSystemConfig } from '@/_utils/cache/system-storage';
import type { SystemConfig } from '@/_utils/config/system';
import type { Locale } from '@/i18n';
import type { RouteRecordRaw } from 'vue-router';

// Erweiterter Zustand inkl. dynamischer Route-Liste
interface State extends SystemConfig {
	routes: RouteRecordRaw[];
}

// Definition des Storeschnittstelle für Setup-Store
export interface SystemStore extends State {
	loading: boolean;
	version: { firmware: string; web: string };
	logging: { state: boolean };
	wlan: State['wlan'];
	serial: State['serial'];
	language: Locale;

	// Aktionen
	getCacheData(): SystemConfig;
	setRoutes(): void;
	setLanguage(lang: Locale): void;
}

export const useSystemStore = defineStore('system', (): SystemStore => {
	// Initial-State
	const state = reactive<State>({
		...systemConfig,
		routes: [],
		loading: false,
	});

	// Persistiere relevanten Teil bei Änderungen
	watch(
		() => ({
			loading: state.loading,
			version: state.version,
			logging: state.logging,
			wlan: { savedNetworks: state.wlan.savedNetworks },
			serial: state.serial,
			language: state.language,
		}),
		(newVal) => setSystemConfig({ ...newVal }),
		{ deep: true }
	);

	// Ref-Wege
	const { loading, version, logging, wlan, serial, language, routes } = toRefs(state);

	function getCacheData(): SystemConfig {
		return {
			loading: state.loading,
			version: state.version,
			logging: state.logging,
			wlan: state.wlan,
			serial: state.serial,
			language: state.language,
		};
	}

	function setRoutes(): void {
		const all = [...constantRoutes, ...developmentRoutes];
		const flatten = (rts: RouteRecordRaw[]): RouteRecordRaw[] => rts.flatMap((r) => (r.children ? flatten(r.children) : []).concat(r));
		state.routes = flatten(all);
	}

	function setLanguage(lang: Locale): void {
		state.language = lang;
	}

	return {
		// State-Refs
		loading: loading.value,
		version: version.value,
		logging: logging.value,
		wlan: wlan.value,
		serial: serial.value,
		language: language.value,
		routes: routes.value,

		// Aktionen
		getCacheData,
		setRoutes,
		setLanguage,
	};
});
