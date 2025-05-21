// src/store/system/index.ts

import { defineStore } from 'pinia';
import { reactive, toRefs, watch } from 'vue';
import { constantRoutes, developmentRoutes } from '@/router';
import { systemConfig } from '@/_utils/config/system';
import { setSystemConfig } from '@/_utils/cache/system-storage';
import type { SystemConfig } from '@/_utils/config/system';
import type { Locale } from '@/i18n';
import type { RouteRecordRaw } from 'vue-router';

interface State extends SystemConfig {
	routes: RouteRecordRaw[];
}

/** Dieses Interface muss exakt dem SystemConfig aus config/system.ts entsprechen */
export interface SystemStore {
	loading: boolean;
	version: { firmware: string; web: string };
	logging: { state: boolean };
	wlan: {
		connection: { status: boolean; connected: boolean; ssid: string; ip: string; gateway: string; subnet: string };
		savedNetworks: Array<{ ssid: string; security: string; channel?: string; rssi?: number }>;
	};
	serial: { available: boolean; baudRate: number; connected: boolean };
	language: Locale;
	routes: RouteRecordRaw[];

	getCacheData(): SystemConfig;
	setRoutes(): void;
	setLanguage(lang: Locale): void;
}

export const useSystemStore = defineStore('system', (): SystemStore => {
	const state = reactive<State>({
		...systemConfig,
		routes: [],
	});

	// Persistiert nur die im Storage erlaubten Felder
	watch(
		() => ({
			loading: state.loading,
			version: state.version,
			logging: state.logging,
			wlan: { savedNetworks: state.wlan.savedNetworks },
			serial: state.serial,
			language: state.language,
		}),
		(newVal) => setSystemConfig(newVal),
		{ deep: true }
	);

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
		// @ts-ignore
		loading,
		// @ts-ignore
		version,
		// @ts-ignore
		logging,
		// @ts-ignore
		wlan,
		// @ts-ignore
		serial,
		// @ts-ignore
		language,
		// @ts-ignore
		routes,

		getCacheData,
		setRoutes,
		setLanguage,
	};
});
