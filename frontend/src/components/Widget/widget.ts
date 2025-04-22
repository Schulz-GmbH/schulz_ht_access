import { defineComponent } from "vue";

// Stores

// composables

// Types

// Components
import WidgetSkeleton from "@/components/Skeleton/skeleton.vue";

// Templates

export default defineComponent({
	name: "Widget",
	components: {
		WidgetSkeleton,
	},
	props: {
		title: { type: String, required: true },
		content: { type: String, default: "" },
		subtext: { type: String, default: "" },
		icon: { type: String, default: "fas fa-info-circle" },
		iconBackground: { type: String, default: "from-blue-700 to-blue-500" },
		isLoading: { type: Boolean, default: false },
		skeletonRows: { type: Number, default: 2 },
		skeletonCols: { type: Number, default: 1 },
		skeletonWithIcon: { type: Boolean, default: true },
	},
	setup() {
		return {};
	},
});
