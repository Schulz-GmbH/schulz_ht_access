// src/store/settings/index.store.ts
import { defineStore } from "pinia";
import { reactive, toRefs, watch } from "vue";
import { systemConfig } from "@/_utils/config/settings.config";
import { setSystemConfig } from "@/_utils/cache/settings.storage.cache";
import { SettingsConfig } from "@/_utils/config/settings.config";

/**
 * Pinia-Store für Layout-Einstellungen.
 * Nutzt ein `reactive` Objekt und `toRefs`, um Typensicherheit und Reaktivität zu gewährleisten.
 */
export const useSettingsStore = defineStore("settings", () => {
	// Reaktiver State basierend auf der aktuellen Layout-Konfiguration
	const state = reactive<SettingsConfig>({
		...systemConfig,
	});

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

	// Exportiere jede Property als Ref plus den Getter
	return {
		...refs,
		getCacheData,
	};
});
