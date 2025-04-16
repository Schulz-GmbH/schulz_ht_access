import { defineComponent, computed } from 'vue'

// Stores
import { useAppStore } from '@/store/app.store'
import { useSettingsStore } from '@/store/settings/index.store'

// composables
// composables
import { useDevice } from '@/_utils/composables/useDevice'
import { useLayoutMode } from '@/_utils/composables/useLayoutMode'

// Types

// Components
import Logo from '@/components/logo/logo.components.vue'

export default defineComponent({
	name: 'Navbar',
	components: { Logo },
	setup() {
		const { isMobile } = useDevice()
		const { isTop } = useLayoutMode()

		const appStore = useAppStore()
		const settingsStore = useSettingsStore()

		const isCollapse = computed(() => !appStore.navbar.opened)
		const isLogo = computed(() => settingsStore.showLogo)

		return {
			isCollapse,
			isLogo,
			isTop,
			isMobile,
		}
	},
})
