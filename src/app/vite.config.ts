import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
// import { resolve } from "path";
import { fileURLToPath, URL } from "node:url";
import { Plugin } from "vite";

// Plugin zum Entfernen des `crossorigin` Attributs
const removeCrossOriginPlugin = (): Plugin => {
	return {
		name: "remove-crossorigin",
		transformIndexHtml(html) {
			return html.replace(/\scrossorigin/g, "");
		},
	};
};

export default defineConfig({
	plugins: [vue(), removeCrossOriginPlugin()],
	resolve: {
		alias: {
			"@": fileURLToPath(new URL("./src", import.meta.url)),
			assets: fileURLToPath(new URL("./src/assets", import.meta.url)), // Alias für assets
		},
	},
	build: {
		outDir: "../../data", // Ändert das Zielverzeichnis zu Lumen's public/dist
		emptyOutDir: true, // Löscht den Ordner vor jedem Build
		rollupOptions: {
			output: {
				entryFileNames: "js/[name].js", // Speichert die Entry-Dateien in einem "js"-Unterordner
				chunkFileNames: "js/[name].js", // Speichert die Chunks in einem "js"-Unterordner
				assetFileNames: (assetInfo) => {
					if (/\.(css)$/.test(assetInfo.name)) {
						return "css/[name][extname]"; // Speichert CSS-Dateien in einem "css"-Unterordner
					}
					return "assets/[name][extname]"; // Für andere Assets
				},
			},
		},
		cssCodeSplit: false, // Verhindert das Splitting von CSS in separate Dateien
		target: "esnext",
	},
	server: {
		port: 3000,
		proxy: {
			"/api": {
				target: "http://localhost:8000", // Dein Lumen-Backend
				changeOrigin: true,
			},
			"/wifi": {
				target: "http://192.168.4.1", // ESP32 AP-Adresse
				changeOrigin: true,
			},
		},
	},
});
