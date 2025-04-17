// src/store/settings/index.store.ts
import { defineStore, type StoreDefinition } from "pinia";
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
export const useSettingsStore: StoreDefinition<"settings", State, {}, Actions> = defineStore("settings", () => {
	// Reaktiver State basierend auf der aktuellen Layout-Konfiguration
	const state = reactive<State>({
		...systemConfig,
		routes: [],
	});

	const routes = constantRoutes;

	// Watcher: Bei jeglicher Änderung im State (deep) speichere aktualisierte Konfig
	watch(
		state,
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
		return { ...state };
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

	// Exportiere jede Property als Ref plus den Getter
	return {
		...refs,
		getCacheData,
		setRoutes,
	};
});
