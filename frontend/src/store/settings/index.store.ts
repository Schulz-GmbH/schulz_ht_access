// src/store/settings/index.store.ts
import { defineStore } from "pinia";
import { reactive, toRefs, watch } from "vue";
import { constantRoutes, developmentRoutes } from "@/router";
import { systemConfig } from "@/_utils/config/settings.config";
import { setSystemConfig } from "@/_utils/cache/settings.storage.cache";
import { SettingsConfig } from "@/_utils/config/settings.config";
import type { RouteRecordRaw } from "vue-router";

/**
 * Erweiterung des SettingsConfig um dynamische Routenlisten
 */
type State = SettingsConfig & {
	routes: RouteRecordRaw[];
};

/**
 * Methoden des Settings-Stores
 */
type Actions = {
	getCacheData: () => SettingsConfig;
	setRoutes: () => void;
};

/**
 * Pinia-Store für Layout-Einstellungen.
 * Nutzt ein `reactive` Objekt und `toRefs`, um Typensicherheit und Reaktivität zu gewährleisten.
 */
export const useSettingsStore = defineStore("system", () => {
	// Reaktiver State basierend auf der aktuellen Layout-Konfiguration
	const state = reactive<State>({
		...systemConfig,
		routes: [],
	});

	// Watcher: Bei jeglicher Änderung im State (deep) speichere aktualisierte Konfig
	watch(
		() => ({ version: state.version, logging: state.logging, wlan: state.wlan }),
		(newVal) => {
			setSystemConfig({ ...newVal });
		},
		{ deep: true },
	);

	// Wandelt alle State-Properties in Ref-Objekte um
	const refs = toRefs(state);

	/**
	 * Getter: Gibt ein flaches Objekt aller aktuellen Settings zurück.
	 */
	function getCacheData(): SettingsConfig {
		return {
			version: state.version,
			logging: state.logging,
			wlan: state.wlan,
		};
	}

	function setRoutes() {
		const all = [...constantRoutes, ...developmentRoutes];

		const flatten = (routes: RouteRecordRaw[]): RouteRecordRaw[] =>
			routes.flatMap((route) => {
				const children = route.children ? flatten(route.children) : [];
				return [route, ...children];
			});

		state.routes = flatten(all);
	}

	// Statt `...toRefs(state)` explizit zurückgeben:
	const { version, logging, wlan } = toRefs(state);

	// Exportiere jede Property als Ref plus den Getter
	return {
		version,
		logging,
		wlan,
		routes: toRefs(state).routes,
		showLogo: toRefs(state as any).showLogo, // oder sicherer: vorher casten
		getCacheData,
		setRoutes,
	};
});
