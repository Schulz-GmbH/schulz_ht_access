import { computed } from 'vue'
import { useSettingsStore } from '@/store/settings/index.store'
import { LayoutModeEnum } from '@/_utils/constants/app-key'

/**
 * Layout-Modus (links / oben / kombiniert)
 */
export function useLayoutMode() {
	const settingsStore = useSettingsStore()

	const isTop = computed(() => settingsStore.layoutMode === LayoutModeEnum.Top)

	function setLayoutMode(mode: LayoutModeEnum) {
		settingsStore.layoutMode = mode
	}

	return {
		isTop,
		setLayoutMode,
	}
}
