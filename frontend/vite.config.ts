/**
 * @file vite.config.ts
 * @brief Vite-Konfiguration für ein Vue3/Vite-Projekt.
 *
 * Detaillierte Beschreibung:
 * Diese Konfiguration bindet das Vue-Plugin ein, fügt ein benutzerdefiniertes Plugin zum Entfernen
 * des `crossorigin` Attributs in der index.html hinzu und definiert diverse Einstellungen für den Build
 * und den Development-Server. Dazu gehören Aliase, Output-Pfade, Dateinamenkonventionen, CSS-Einstellungen
 * und Proxy-Konfigurationen für WebSocket und HTTP.
 */

import { defineConfig, loadEnv, Plugin } from "vite";
import vue from "@vitejs/plugin-vue";
// import { resolve } from "path";
import { fileURLToPath, URL } from "node:url";

/**
 * @brief Plugin zum Entfernen des `crossorigin` Attributs aus der index.html.
 *
 * Dieses Plugin durchsucht den HTML-Code und entfernt alle Vorkommen des Attributes `crossorigin`,
 * indem es die entsprechende Zeichenkette ersetzt.
 *
 * @return {Plugin} Ein Vite-Plugin, das den HTML-Inhalt transformiert.
 */
const removeCrossOriginPlugin = (): Plugin => {
	return {
		name: "remove-crossorigin",
		/**
		 * @brief Transformiert den HTML-Inhalt, um das `crossorigin` Attribut zu entfernen.
		 *
		 * @param {string} html Der ursprüngliche HTML-Code.
		 * @return {string} Der modifizierte HTML-Code ohne `crossorigin` Attribute.
		 */
		transformIndexHtml(html) {
			return html.replace(/\scrossorigin/g, "");
		},
	};
};

/**
 * @brief Vite-Konfiguration basierend auf dem aktuellen Modus.
 *
 * Lädt die Umgebungsvariablen anhand des aktuellen Modus und konfiguriert Vite
 * mit folgenden Parametern:
 * - Plugins: Einbindung des Vue-Plugins und des benutzerdefinierten removeCrossOriginPlugin.
 * - resolve: Definition von Pfad-Aliasen, z. B. für das Verzeichnis "src" und "src/assets".
 * - build: Konfiguration des Output-Verzeichnisses, Dateinamenkonventionen sowie CSS-Verhalten.
 * - server: Einstellungen für den Entwicklungsserver, wie Port und Proxy-Konfigurationen für WebSocket
 *   und HTTP-Anfragen.
 *
 * @param {object} param0 Kontextobjekt, das unter anderem den aktuellen Modus enthält.
 * @return {object} Das Vite-Konfigurationsobjekt.
 */
export default defineConfig(({ mode }) => {
	// Laden der Umgebungsvariablen basierend auf dem aktuellen Modus
	const env = loadEnv(mode, process.cwd(), "");

	return {
		plugins: [
			// Einbindung des Vue-Plugins
			vue(),
			// Einbindung des Plugins zum Entfernen von `crossorigin`
			removeCrossOriginPlugin(),
		],
		resolve: {
			alias: {
				"@": fileURLToPath(new URL("./src", import.meta.url)),
				assets: fileURLToPath(new URL("./src/assets", import.meta.url)), ///< Alias für das assets-Verzeichnis
			},
		},
		build: {
			/**
			 * @brief Zielverzeichnis für den Build.
			 *
			 * Das Output-Verzeichnis wird auf "../../data" gesetzt, um beispielsweise Lumen's
			 * public/dist zu adressieren.
			 */
			outDir: "../../data",
			/**
			 * @brief Leert das Zielverzeichnis vor jedem Build.
			 *
			 * Damit wird sichergestellt, dass nur die aktuellen Build-Artefakte im Output-Verzeichnis vorhanden sind.
			 */
			emptyOutDir: true,
			rollupOptions: {
				output: {
					/**
					 * @brief Dateinamen-Konvention für die Entry-Dateien.
					 *
					 * Speichert die generierten Entry-Dateien in einem "js"-Unterordner.
					 */
					entryFileNames: "js/[name].js",
					/**
					 * @brief Dateinamen-Konvention für Chunks.
					 *
					 * Speichert die Chunks in einem "js"-Unterordner.
					 */
					chunkFileNames: "js/[name].js",
					/**
					 * @brief Bestimmt die Dateinamen für Assets.
					 *
					 * CSS-Dateien werden in einem "css"-Unterordner gespeichert, andere Assets in einem
					 * "assets"-Unterordner.
					 *
					 * @param {object} assetInfo Informationen zum Asset, insbesondere der Name.
					 * @return {string} Der formatierte Pfad inkl. Dateinamen und Endung.
					 */
					assetFileNames: (assetInfo) => {
						if (/\.(css)$/.test(assetInfo.name)) {
							return "css/[name][extname]";
						}
						return "assets/[name][extname]";
					},
				},
			},
			/**
			 * @brief Deaktiviert das Splitting von CSS in separate Dateien.
			 *
			 * Dies sorgt dafür, dass CSS in die JavaScript-Dateien eingebettet wird.
			 */
			cssCodeSplit: false,
			/**
			 * @brief JavaScript-Target.
			 *
			 * Setzt das Target auf "esnext", um moderne JavaScript-Features ohne Transpilierung zu nutzen.
			 */
			target: "esnext",
		},
		server: {
			/**
			 * @brief Port-Konfiguration für den Development-Server.
			 */
			port: 3000,
			proxy: {
				/**
				 * @brief Proxy-Konfiguration für WebSocket-Verbindungen.
				 *
				 * Leitet Anfragen an "/ws" an die angegebene ESP32 WebSocket-URL weiter.
				 * Wichtige Parameter:
				 * - ws: Aktiviert den WebSocket-Proxy.
				 * - changeOrigin: Umgeht CORS-Probleme.
				 * - rewrite: Erlaubt die Anpassung des Pfads.
				 */
				"/ws": {
					target: "ws://192.168.178.52", // ESP32 WebSocket-URL
					ws: true,
					changeOrigin: true,
					rewrite: (path) => path.replace(/^\/ws/, "/ws"),
				},
				/**
				 * @brief Proxy-Konfiguration für HTTP-Anfragen.
				 *
				 * Leitet Anfragen an "/logfile" an den in den Umgebungsvariablen definierten ESP32 HTTP-Server weiter.
				 * Mit changeOrigin wird sichergestellt, dass keine CORS-Probleme auftreten.
				 */
				"/logfile": {
					target: env.VITE_API_URL, // ESP32 HTTP-Server
					changeOrigin: true,
				},
			},
		},
	};
});
