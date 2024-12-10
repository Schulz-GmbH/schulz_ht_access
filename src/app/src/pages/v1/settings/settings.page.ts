import { ref, onMounted, defineComponent } from "vue";

// Mixins
import LayoutMixin from "@/mixins/layout.mixin";

// Services
import { WiFiService } from "@/services/wifi.service";

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
	mixins: [LayoutMixin],
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
				const data = await WiFiService.fetchWiFiConfig();
				existingWiFiData.value = data;
				wifiData.value.ssid = data.ssid || ""; // Set initial data if available
				wifiData.value.password = "******";
			} catch (error) {
				console.error("Fehler beim Abrufen der WLAN-Daten:", error);
				message.value = "Fehler beim Abrufen der WLAN-Daten.";
				isError.value = true;
			}
		};

		const submitWiFiConfig = async () => {
			try {
				const statusMessage = await WiFiService.submitWiFiConfig(
					wifiData.value
				);
				message.value = statusMessage;
				isError.value = false;
			} catch (error) {
				if (error instanceof Error) {
					console.error(
						"Fehler beim Speichern der WLAN-Daten:",
						error
					);
					message.value = error.message;
				} else {
					console.error(
						"Unbekannter Fehler beim Speichern der WLAN-Daten:",
						error
					);
					message.value = "Ein unbekannter Fehler ist aufgetreten.";
				}
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
	methods: {
		getLayoutClass() {
			return "grid-cols-4 sm:grid-cols-[100%]";
		},
	},
	mounted() {},
});
