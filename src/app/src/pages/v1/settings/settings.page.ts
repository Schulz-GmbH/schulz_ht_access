import { ref, onMounted, defineComponent } from "vue";

// Mixins
import LayoutMixin from "@/mixins/layout.mixin";

// Services
import { WifiService } from "@/services/wifi/wifi.service";
import { SystemService } from "@/services/system/system.service";

export default defineComponent({
	name: "SettingsPage",
	components: {},
	mixins: [LayoutMixin],
	setup() {
		const wifiData = ref({
			ssid: "",
			password: "",
		});

		const apiData = ref({
			ip: "",
			port: 9000,
		});

		// State
		const wifiResponse = ref<{ status: string; details?: string } | null>(
			null
		);

		const updateResponse = ref<{ status: string; details?: string } | null>(
			null
		);

		const isLoading = ref<boolean>(false);
		const error = ref<{ message: string; code?: number } | null>(null);

		// Hilfsfunktion für Requests
		const handleRequest = async <T>(
			request: () => Promise<T>
		): Promise<T | null> => {
			isLoading.value = true;
			error.value = null;

			try {
				const result = await request();
				return result;
			} catch (err) {
				error.value = {
					message:
						err instanceof Error
							? err.message
							: "Unbekannter Fehler",
				};
				return null;
			} finally {
				isLoading.value = false;
			}
		};

		const fetchWlan = async () => {
			const response = await handleRequest(WifiService.getCredentials);
			if (response && response.length > 0) {
				const firstCredential = response[0]; // Zugriff auf das erste Element des Arrays
				wifiData.value.ssid = firstCredential.ssid || "";
			} else {
				console.error("No credentials received");
			}
		};

		const fetchSystem = async () => {
			const response = await handleRequest(SystemService.getApiServer);
			if (response) {
				const api = response[0]; // Zugriff auf das erste Element des Arrays
				apiData.value.ip = api.ip || "";
				apiData.value.port = Number(api.port) || 9000;
			}
			// startUpdateTimer();
		};

		const validateIPAddress = (): boolean => {
			const ipRegex =
				/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;

			if (!ipRegex.test(apiData.value.ip)) {
				// Setze den Fehler in die zentrale `error`-State-Variable
				error.value = {
					message:
						"Bitte eine gültige IP-Adresse eingeben (z. B. 192.168.1.1).",
					code: 400, // Optional: HTTP-Status oder ein eigener Fehlercode
				};
				return false;
			} else {
				// Lösche den Fehler, wenn die Eingabe gültig ist
				error.value = null;
				return true;
			}
		};

		// Lifecycle hooks
		onMounted(() => {
			fetchWlan();
			fetchSystem();
		});

		return {
			isLoading,
			error,
			fetchWlan,
			wifiData,
			apiData,
			validateIPAddress,
		};
	},
	methods: {
		getLayoutClass() {
			return "grid-cols-6 sm:grid-cols-[100%] grid-rows-4";
		},
	},
	mounted() {},
});

//
//
// import { ref, onMounted, defineComponent } from "vue";

// // Mixins
// import LayoutMixin from "@/mixins/layout.mixin";

// // Services
// import { WiFiService } from "@/services/wifi.service";

// // Components
// import WlanWidget from "@/components/widgets/wlan/wlan.widget.vue";
// import VersionWidget from "@/components/widgets/version/version.widget.vue";

// export default defineComponent({
// 	name: "SettingsPage",
// 	components: {
// 		WlanWidget: WlanWidget,
// 		VersionWidget: VersionWidget,
// 	},
// 	mixins: [LayoutMixin],
// 	setup() {
// 		// WLAN Daten
// 		const wifiData = ref({
// 			ssid: "",
// 			password: "",
// 		});

// 		// Update Server Daten
// 		const serverData = ref({
// 			ip: "",
// 			port: "",
// 		});

// 		// Bestehende Daten (falls vorhanden)
// 		const existingWiFiData = ref({});
// 		const existingServerData = ref({});

// 		// Nachrichten und Fehlerstatus für WLAN
// 		const wifiMessage = ref("");
// 		const wifiIsError = ref(false);

// 		// Nachrichten und Fehlerstatus für Update Server
// 		const serverMessage = ref("");
// 		const serverIsError = ref(false);

// 		// Funktion zur Validierung der IP-Adresse
// 		const validateIPAddress = () => {
// 			const ipRegex =
// 				/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
// 			if (!ipRegex.test(serverData.value.ip)) {
// 				serverMessage.value =
// 					"Bitte eine gültige IP-Adresse eingeben (z. B. 192.168.1.1).";
// 				serverIsError.value = true;
// 				return false;
// 			} else {
// 				serverMessage.value = "";
// 				serverIsError.value = false;
// 				return true;
// 			}
// 		};

// 		// WLAN-Daten abrufen
// 		const fetchWiFiConfig = async () => {
// 			try {
// 				WiFiService.fetchWiFiConfig()
// 					.then((config) => {
// 						wifiData.value.ssid = config.ssid || "";
// 						wifiData.value.password = "******";
// 					})
// 					.catch((error) => console.error(error.message));
// 			} catch (error) {
// 				console.error("Fehler beim Abrufen der WLAN-Daten:", error);
// 				wifiMessage.value = "Fehler beim Abrufen der WLAN-Daten.";
// 				wifiIsError.value = true;
// 			}
// 		};

// 		// WLAN-Daten speichern
// 		const submitWiFiConfig = async () => {
// 			try {
// 				const statusMessage = await WiFiService.submitWiFiConfig(
// 					wifiData.value
// 				);
// 				wifiMessage.value = statusMessage;
// 				wifiIsError.value = false;
// 			} catch (error) {
// 				wifiMessage.value =
// 					error.message || "Ein Fehler ist aufgetreten.";
// 				wifiIsError.value = true;
// 			}
// 		};

// 		// Server-Daten speichern
// 		const submitServerConfig = async () => {
// 			if (!validateIPAddress()) {
// 				return; // Validierung fehlgeschlagen, nicht speichern
// 			}
// 			serverMessage.value = "Server-Konfiguration gespeichert!";
// 			serverIsError.value = false;
// 		};

// 		onMounted(() => {
// 			fetchWiFiConfig();
// 		});

// 		return {
// 			// WLAN
// 			wifiData,
// 			existingWiFiData,
// 			wifiMessage,
// 			wifiIsError,
// 			submitWiFiConfig,

// 			// Update Server
// 			serverData,
// 			existingServerData,
// 			serverMessage,
// 			serverIsError,
// 			validateIPAddress,
// 			submitServerConfig,
// 		};
// 	},
// 	methods: {
// 		getLayoutClass() {
// 			return "grid-cols-4 sm:grid-cols-[100%]";
// 		},
// 	},
// 	mounted() {},
// });
