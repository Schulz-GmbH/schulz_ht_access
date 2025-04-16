// src/store/settings/index.store.ts
import type { LayoutsConfig } from "@/layouts/_config";
import { layoutsConfig } from "@/layouts/_config";
import { defineStore } from "pinia";
import { reactive, toRefs, watch } from "vue";
import { setLayoutsConfig } from "@/_utils/cache/local-storage";

/**
 * Pinia-Store für Layout-Einstellungen.
 * Nutzt ein `reactive` Objekt und `toRefs`, um Typensicherheit und Reaktivität zu gewährleisten.
 */
export const useSettingsStore = defineStore("settings", () => {
	// Reaktiver State basierend auf der aktuellen Layout-Konfiguration
	const state = reactive<LayoutsConfig>({
		...layoutsConfig,
	});

	// Watcher: Bei jeglicher Änderung im State (deep) speichere aktualisierte Konfig
	watch(
		state,
		(newVal) => {
			setLayoutsConfig({ ...newVal });
		},
		{ deep: true },
	);

	// Wandelt alle State-Properties in Ref-Objekte um
	const refs = toRefs(state);

	/**
	 * Getter: Gibt ein flaches Objekt aller aktuellen Settings zurück.
	 */
	function getCacheData(): LayoutsConfig {
		return { ...state };
	}

	// Exportiere jede Property als Ref plus den Getter
	return {
		...refs,
		getCacheData,
	};
});
