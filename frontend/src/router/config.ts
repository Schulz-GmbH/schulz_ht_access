import type { RouterHistory } from 'vue-router'
import { createWebHashHistory, createWebHistory } from 'vue-router'

/** Router-Konfiguration */
interface RouterConfig {
	history: RouterHistory
	dynamic: boolean
	thirdLevelRouteCache: boolean
}

const VITE_ROUTER_HISTORY = import.meta.env.VITE_ROUTER_HISTORY || 'history'
const VITE_PUBLIC_PATH = import.meta.env.VITE_PUBLIC_PATH

export const routerConfig: RouterConfig = {
	history:
		VITE_ROUTER_HISTORY === 'hash'
			? createWebHashHistory(VITE_PUBLIC_PATH)
			: createWebHistory(VITE_PUBLIC_PATH),
	dynamic: true,
	thirdLevelRouteCache: false,
}
