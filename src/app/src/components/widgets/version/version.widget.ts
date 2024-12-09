import { defineComponent, ref, onMounted, onUnmounted, computed } from "vue";

// Services
import { VersionService } from "@/components/widgets/version/version.service";

// Helper
import { useLastUpdated } from "@/helpers/updateHelper";

// Layouts

// Componetns

/**
 * Version Widget - The root Vue component of the widget.
 *
 * This component is responsible for initializing the application's state
 * and setting up the main layout.
 */
import Widget from "@/components/widgets/widget.vue";

export default defineComponent({
	name: "VersionWidget",
	components: {
		Widget,
	},
	props: {
		title: {
			type: String,
			default: "Version Widget",
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
		const appVersion = ref(import.meta.env.VITE_APP_VERSION);
		const isUpdateAvailable = ref(false);
		const isLoading = ref(true);
		const fetchError = ref<string | null>(null);

		const { lastUpdated, updateText, startUpdateTimer } = useLastUpdated();

		// Fetch version on component mount
		onMounted(async () => {
			try {
				const data = await VersionService.fetchVersion();
				isUpdateAvailable.value = data.updateAvailable;
				lastUpdated.value = new Date();
			} catch (error) {
				const errMessage =
					error instanceof Error ? error.message : String(error);
				fetchError.value = errMessage;
			} finally {
				isLoading.value = false;
			}

			startUpdateTimer();
		});

		return {
			appVersion,
			isUpdateAvailable,
			isLoading,
			fetchError,
			updateText,
		};
	},
});
