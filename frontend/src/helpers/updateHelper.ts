import { ref, computed, onUnmounted } from "vue";

export function useLastUpdated() {
	const lastUpdated = ref<Date | null>(null);
	let updateTimer: NodeJS.Timeout | null = null;

	const updateText = computed(() => {
		if (!lastUpdated.value) return "Last updated: just now";

		const now = new Date();
		const diffMs = now.getTime() - lastUpdated.value.getTime();
		const diffMinutes = Math.floor(diffMs / 60000);

		if (diffMinutes === 0) return "Last updated: just now";
		if (diffMinutes === 1) return "Last updated: 1 minute ago";
		return `Last updated: ${diffMinutes} minutes ago`;
	});

	const startUpdateTimer = () => {
		updateTimer = setInterval(() => {
			// Forces re-evaluation of computed properties
			lastUpdated.value = lastUpdated.value
				? new Date(lastUpdated.value)
				: null;
		}, 60000);
	};

	// Clean up the interval timer on component unmount
	onUnmounted(() => {
		if (updateTimer) {
			clearInterval(updateTimer);
		}
	});

	return {
		lastUpdated,
		updateText,
		startUpdateTimer,
	};
}
