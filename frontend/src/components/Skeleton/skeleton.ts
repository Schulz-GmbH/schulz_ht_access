import { defineComponent, computed } from "vue";

// Stores

// composables

// Types

// Components

// Templates

export default defineComponent({
	name: "WidgetSkeleton",
	props: {
		isLoading: { type: Boolean, required: true },
		rows: { type: Number, default: 2 },
		cols: { type: Number, default: 1 },
		withIcon: { type: Boolean, default: true },
	},
	setup(props) {
		const randomWidths = computed(() => {
			return Array.from({ length: props.rows }, () => {
				// Zufällige Breite zwischen 30% und 100%
				return `${Math.floor(Math.random() * 40 + 60)}%`;
			});
		});

		return {
			randomWidths,
		};
	},
});
