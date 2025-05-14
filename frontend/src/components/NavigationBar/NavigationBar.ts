import { defineComponent, computed, onMounted } from "vue";

// Stores
import { useAppStore } from "@/store/app.store";

// Composables
import { useDevice } from "@/_utils/composables/useDevice";

// Types

// Components
import Breadcrumb from "../Breadcrumb/breadcrumb.vue";
import Hamburger from "../Hamburger/Hamburger.vue";

export default defineComponent({
	name: "NavigationBar",
	components: { Breadcrumb, Hamburger },
	setup() {
		const appStore = useAppStore();
		const { isMobile } = useDevice();

		/** Seitenleiste umschalten */
		function toggleSidebar() {
			appStore.toggleNavbar(false);
		}

		return { appStore, isMobile, toggleSidebar };
	},
});
