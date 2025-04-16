import type { LayoutsConfig } from "@/layouts/_config";
import { layoutsConfig } from "@/layouts/_config";
import { defineStore } from "pinia";
import { ref, watch, type Ref } from "vue";
import { setLayoutsConfig } from "@/_utils/cache/local-storage";

/**
 * SettingsStore: Jeder Schlüssel von `LayoutsConfig` ist ein `Ref<T>`.
 */
type SettingsStore = {
	[K in keyof LayoutsConfig]: Ref<LayoutsConfig[K]>;
};

/**
 * Pinia Store für die Layout-Einstellungen.
 */
export const useSettingsStore = defineStore("settings", () => {
	// Reaktiver Zustand für die Layout-Einstellungen
	const state = {} as SettingsStore;

	// Durchlaufe alle Schlüssel von `layoutsConfig` und mache sie reaktiv
	(Object.keys(layoutsConfig) as Array<keyof LayoutsConfig>).forEach((key) => {
		// Sichere Typenkonvertierung von `layoutsConfig[key]`
		const value = layoutsConfig[key] as LayoutsConfig[typeof key];

		// Erstellt ein reaktives Ref-Objekt für jeden Schlüssel
		const refValue: Ref<LayoutsConfig[typeof key]> = ref(value);
		state[key] = refValue;

		// Überwache Änderungen und speichere die neuen Einstellungen
		watch(refValue, () => {
			setLayoutsConfig(getCacheData());
		});
	});

	/**
	 * Erstellt ein Objekt mit den aktuellen Einstellungen für das Caching.
	 * @returns {LayoutsConfig} Das gecachte Layout-Objekt.
	 */
	const getCacheData = (): LayoutsConfig => {
		const settings = {} as LayoutsConfig;
		(Object.keys(state) as Array<keyof LayoutsConfig>).forEach((key) => {
			// Sicherstellen, dass TypeScript den richtigen Typ kennt
			settings[key] = state[key].value as LayoutsConfig[typeof key];
		});
		return settings;
	};

	return state;
});
