console.log('📦 MAIN LOADED')

import './assets/main.css'

import { createApp } from 'vue'
import { pinia } from '@/store/index'

import App from './app.main.vue'
import { router } from './router'

import '@/assets/main.css'

/**
 * Main entry point of the Vue application.
 *
 * This file is responsible for bootstrapping the Vue application.
 * It imports the main App component, as well as the router and store modules,
 * and then creates and mounts the Vue application.
 */

/**
 * Import and initialize the Vue application.
 *
 * - `createApp` function from Vue is used to create a new Vue application instance.
 * - `App` is the root component of the application.
 * - `router` is the Vue Router instance for handling navigation.
 * - `store` is the Vuex store instance for state management.
 */
const app = createApp(App)

/**
 * Configure the application with plugins and global components.
 *
 * - `use` function is used to install plugins (router and store) into the Vue application.
 */
app.use(pinia)
app.use(router)

/**
 * Mount the Vue application to a DOM element.
 *
 * - `mount` function is used to mount the Vue application to the DOM.
 * - Here, the application is mounted to the 'body' element.
 */
router.isReady().then(() => {
	app.mount('#app')
})

document.addEventListener('DOMContentLoaded', () => {
	console.log('✅ DOM ready')
})
