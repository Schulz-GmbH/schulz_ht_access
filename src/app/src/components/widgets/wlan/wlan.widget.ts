import { defineComponent, ref, onMounted, onUnmounted, computed } from "vue";

// Services
import { WlanService } from "@/components/widgets/wlan/wlan.service";

// Helper
import { useLastUpdated } from "@/helpers/updateHelper";

// Layouts

// Components

/**
 * WLAN Widget - The root Vue component of the widget.
 *
 * This component is responsible for initializing the application's state
 * and setting up the main layout.
 */
import Widget from "@/components/widgets/widget.vue";

export default defineComponent({
	name: "WlanWidget",
	components: {
		Widget,
	},
	props: {
		title: {
			type: String,
			default: "WLAN Widget",
		},
		footerText: {
			type: String,
			default: "Last updated: just now",
		},
		actionLabel: {
			type: String,
			default: "Action",
		},
	},
	setup() {
		const wlanStatus = ref<boolean>(false);
		const { lastUpdated, updateText, startUpdateTimer } = useLastUpdated();

		const wlanStatusText = computed(() =>
			wlanStatus.value ? "An" : "Aus"
		);

		// WebSocket einrichten
		onMounted(async () => {
			WlanService.connect();

			try {
				wlanStatus.value = await WlanService.getStatus();
				lastUpdated.value = new Date();
			} catch (error) {
				console.error("Fehler beim Abrufen des WLAN-Status:", error);
			}

			WlanService.onStatusChange((status: boolean) => {
				wlanStatus.value = status;
				lastUpdated.value = new Date();
			});

			// Start timer to refresh footerText
			startUpdateTimer();
		});

		// Socket bei der Zerstörung der Komponente schließen
		onUnmounted(() => {
			WlanService.disconnect();
		});

		// WLAN Status umschalten
		const toggleWlan = async () => {
			try {
				await WlanService.toggleStatus();
				lastUpdated.value = new Date();
			} catch (error) {
				console.error("Fehler beim Umschalten des WLAN-Status:", error);
			}
		};

		return {
			wlanStatus,
			wlanStatusText,
			updateText,
			toggleWlan,
		};
	},
});
