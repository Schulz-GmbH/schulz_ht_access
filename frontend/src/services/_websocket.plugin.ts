// websocket.plugin.ts
import { App } from "vue";
import { SocketService } from "@/services/_socket.service";

export default {
	install: (app: App, options: { url: string }) => {
		// Initialisiere den WebSocket-Service
		SocketService.connect(options.url)
			.then(() => {
				console.log("WebSocket verbunden mit", options.url);
			})
			.catch((error) => {
				console.error("WebSocket-Verbindung fehlgeschlagen:", error);
			});

		// Füge den WebSocket-Service zur globalen Instanz hinzu
		app.config.globalProperties.$socket = SocketService;
	},
};
