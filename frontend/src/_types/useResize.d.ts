// src/layouts/composables/useResize.ts

import { onBeforeMount, onMounted, onBeforeUnmount } from 'vue'
import { useAppStore } from '@/store/app.store'
import { useRouteListener } from '@/_utils/composables/useRouteListener'
import { DeviceEnum } from '@/_utils/constants/app-key'

/**
 * Maximale Breite für mobile Geräte
 */
const MAX_MOBILE_WIDTH = 992

/**
 * Passt das Layout je nach Browserbreite dynamisch an
 */
export function useResize(): void {
	const appStore = useAppStore()
	const { listenerRouteChange } = useRouteListener()

	const isMobile = (): boolean => {
		const rect = document.body.getBoundingClientRect()
		return rect.width - 1 < MAX_MOBILE_WIDTH
	}

	const resizeHandler = (): void => {
		if (!document.hidden) {
			const _isMobile = isMobile()
			appStore.toggleDevice(_isMobile ? DeviceEnum.Mobile : DeviceEnum.Desktop)
			if (_isMobile) appStore.closeNavbar(true)
		}
	}

	listenerRouteChange(() => {
		if (appStore.device === DeviceEnum.Mobile && appStore.navbar.opened) {
			appStore.closeNavbar(false)
		}
	})

	onBeforeMount(() => {
		window.addEventListener('resize', resizeHandler)
	})

	onMounted(() => {
		if (isMobile()) {
			appStore.toggleDevice(DeviceEnum.Mobile)
			appStore.closeNavbar(true)
		}
	})

	onBeforeUnmount(() => {
		window.removeEventListener('resize', resizeHandler)
	})
}
