import { fileURLToPath, URL } from 'node:url';

import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';
import vueDevTools from 'vite-plugin-vue-devtools';
import tailwindcss from '@tailwindcss/vite';
import vueI18n from '@intlify/unplugin-vue-i18n/vite';
import { VitePWA } from 'vite-plugin-pwa';
import path from 'path';

// https://vite.dev/config/
export default defineConfig(({ mode }) => ({
	plugins: [
		vue(),
		vueDevTools(),
		tailwindcss(),
		vueI18n({
			include: path.resolve(__dirname, './src/locales/**'),
		}),
		VitePWA({
			// 1) auch im Dev-Modus aktivieren
			devOptions: {
				enabled: true, // Service Worker beim `npm run dev` erzeugen
				type: 'module', // als ES‑Module, damit moderne Browser es nehmen
			},
			// 2) Name der SW‑Datei (wird im root abgelegt)
			filename: 'sw.js',
			// 3) Generiere einen Service Worker automatisch
			strategies: 'generateSW',
			registerType: 'autoUpdate',
			// 4) Precaching: welche Dateien aus dem Build automatisch rein?
			workbox: {
				globPatterns: mode === 'production' ? ['**/*.{js,css,html,png,svg,ico}'] : [],
				// Navigation-Fallback, damit index.html bei Offline immer geladen wird
				navigateFallback: '/index.html',
				// Optional: Laufzeit-Regeln für spezielle Routen
				runtimeCaching: [
					{
						urlPattern: /^\/seite1/,
						handler: 'NetworkFirst',
						options: {
							cacheName: 'seite1-cache',
							expiration: { maxEntries: 5, maxAgeSeconds: 60 * 60 * 24 },
						},
					},
					{
						urlPattern: /^\/api\//,
						handler: 'NetworkOnly',
					},
				],
			},
			includeAssets: ['favicon.svg', 'favicon.ico', 'robots.txt', 'apple-touch-icon.png'],
			manifest: {
				name: 'Schulz GmbH - IOT Access',
				short_name: 'Schulz',
				description: 'Offline-fähige Anwendung für Schulz GmbH - IOT Access',
				theme_color: '#ffffff',
				background_color: '#ffffff',
				display: 'standalone',
				icons: [
					{
						src: '/icon-72x72.png',
						sizes: '72x72',
						type: 'image/png',
					},
					{
						src: '/icon-96x96.png',
						sizes: '96x96',
						type: 'image/png',
					},
					{
						src: '/icon-128x128.png',
						sizes: '128x128',
						type: 'image/png',
					},
					{
						src: '/icon-144x144.png',
						sizes: '144x144',
						type: 'image/png',
					},
					{
						src: '/icon-152x152.png',
						sizes: '152x152',
						type: 'image/png',
					},
					{
						src: '/icon-192x192.png',
						sizes: '192x192',
						type: 'image/png',
					},
					{
						src: '/icon-384x384.png',
						sizes: '384x384',
						type: 'image/png',
					},
					{
						src: '/icon-512x512.png',
						sizes: '512x512',
						type: 'image/png',
					},
				],
				screenshots: [
					{
						src: '/screenshot-mobile.png',
						sizes: '640x1136',
						type: 'image/png',
					},
					{
						src: '/screenshot-desktop.png',
						sizes: '1024x768',
						type: 'image/png',
						form_factor: 'wide',
					},
				],
			},
		}),
	],
	resolve: {
		alias: {
			'@': fileURLToPath(new URL('./src', import.meta.url)),
			assets: fileURLToPath(new URL('./src/assets', import.meta.url)),
		},
	},
	server: {
		host: true,
		port: 5173,
	},
	build: {
		outDir: '../firmware/data/www/html',
		assetsDir: 'assets',
		sourcemap: false,
		rollupOptions: {
			output: {
				entryFileNames: `[name].js`, // statt index-B94evXZl.js → index.js
				chunkFileNames: `[name].js`, // statt routes.page-Dxep2V_q.js → routes.page.js
				assetFileNames: `[name][extname]`, // statt fa-solid-900-CTAAxXor.woff2 → fa-solid-900.woff2
			},
		},
	},
}));
