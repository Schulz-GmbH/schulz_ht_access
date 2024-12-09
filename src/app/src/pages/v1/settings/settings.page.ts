import { ref, onMounted, defineComponent } from "vue";
import axios from "axios";

// Layouts

// Componetns
import WlanWidget from "@/components/widgets/wlan/wlan.widget.vue";
import VersionWidget from "@/components/widgets/version/version.widget.vue";

/**
 * App - The root Vue component of the application.
 *
 * This component is responsible for initializing the application's state
 * and setting up the main layout.
 */
export default defineComponent({
	name: "SettingsPage",
	components: {
		WlanWidget: WlanWidget,
		VersionWidget: VersionWidget,
	},
	data() {
		return {};
	},
	setup() {
		const wifiData = ref({
			ssid: "",
			password: "",
		});

		const existingWiFiData = ref<{ ssid?: string }>({});
		const message = ref("");
		const isError = ref(false);

		const fetchWiFiConfig = async () => {
			try {
				const response = await axios.get("http://192.168.5.1/wifi", {
					headers: {
						"Content-Type": "application/json",
					},
				});
				existingWiFiData.value = response.data;
				wifiData.value.ssid = response.data.ssid || ""; // Setze initiale Daten, falls vorhanden
				wifiData.value.password = "******";
			} catch (error) {
				console.error("Fehler beim Abrufen der WLAN-Daten:", error);
				message.value = "Fehler beim Abrufen der WLAN-Daten.";
				isError.value = true;
			}
		};

		const submitWiFiConfig = async () => {
			try {
				const response = await axios.post(
					"http://192.168.5.1/wifi",
					wifiData.value,
					{
						headers: {
							"Content-Type": "application/json",
						},
					}
				);
				message.value =
					response.data.status || "Erfolgreich gespeichert!";
				isError.value = false;
			} catch (error: any) {
				console.error("Fehler beim Speichern der WLAN-Daten:", error);
				message.value =
					error.response?.data?.error ||
					"Fehler beim Speichern der WLAN-Daten.";
				isError.value = true;
			}
		};

		onMounted(() => {
			fetchWiFiConfig();
		});

		return {
			wifiData,
			existingWiFiData,
			message,
			isError,
			submitWiFiConfig,
		};
	},
	methods: {},
	mounted() {},
});
