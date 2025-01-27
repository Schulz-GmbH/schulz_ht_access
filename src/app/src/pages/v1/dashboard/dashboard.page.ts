import { defineComponent, ref, onMounted, onUnmounted, computed } from "vue";

// Mixins
import LayoutMixin from "@/mixins/layout.mixin";

// Layouts

// Componetns

// Services
import { SystemService } from "@/services/system/system.service";
import { WifiService } from "@/services/wifi/wifi.service";

// Helper
import { useLastUpdated } from "@/helpers/updateHelper";

/**
 * App - The root Vue component of the application.
 *
 * This component is responsible for initializing the application's state
 * and setting up the main layout.
 */
export default defineComponent({
	name: "DashboardPage",
	components: {},
	mixins: [LayoutMixin],
	data() {
		return {};
	},
	setup() {
		// State
		const wifiResponse = ref<{ status: string; details?: string } | null>(
			null
		);
		const systemVersion = ref<string | null>(null);
		const serverVersion = ref<string | null>(null);
		const available = ref<string | null>(null);
		const isLoading = ref<boolean>(false);
		const error = ref<{ message: string; code?: number } | null>(null);
		const wlanStatus = ref<"connected" | "disconnected">("disconnected");

		// Computed
		const wlanStatusText = computed(() =>
			wlanStatus.value === "connected" ? "Verbunden" : "Getrennt"
		);

		const { lastUpdated, updateText, startUpdateTimer } = useLastUpdated();

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

		// Fetch-Funktionen
		const fetchSystem = async () => {
			const response = await handleRequest(SystemService.getVersion);
			if (response) {
				systemVersion.value = response[0]["system-version"];
				serverVersion.value = response[0]["server-version"];
				available.value = response[0]["available"];
				lastUpdated.value = new Date();
			}
			startUpdateTimer();
		};

		const fetchWlan = async () => {
			const response = await handleRequest(WifiService.getStatus);
			if (response) {
				wifiResponse.value = response;
				wlanStatus.value = response.status;
			}
		};

		// Lifecycle hooks
		onMounted(() => {
			fetchSystem();
			fetchWlan();
		});

		onUnmounted(() => {});

		return {
			available,
			serverVersion,
			systemVersion,
			isLoading,
			error,
			fetchSystem,
			updateText,
			fetchWlan,
			wlanStatus,
			wlanStatusText,
		};
	},
	methods: {
		getLayoutClass() {
			return "grid-cols-6 sm:grid-cols-[100%] grid-rows-4";
		},
	},
	mounted() {},
});
