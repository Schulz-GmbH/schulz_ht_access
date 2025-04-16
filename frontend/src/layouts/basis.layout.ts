import { defineComponent, defineAsyncComponent } from "vue";

// Stores

// composables
import { useResize } from "@/layouts/composables/useResize";

// Types

// Components

// Templates

export default defineComponent({
	name: "BasisLayout",
	components: {
		OfflineComponent: defineAsyncComponent(() => import("@/components/offline/offline.component.vue")),
		AppLayout: defineAsyncComponent(() => import("@/layouts/app.layout.vue")),
	},
	data() {
		return {
			// data
		};
	},
	setup() {
		// Layout responsiv machen
		useResize();
		// setup
	},
	// computed
});
