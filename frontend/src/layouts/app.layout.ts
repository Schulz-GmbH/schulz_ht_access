import { defineComponent, computed, onMounted } from "vue";

// Stores
import { useAppStore } from "@/store/app.store";

// composables
import { useDevice } from "@/_utils/composables/useDevice";

// Types

// Components
import NavigationBar from "@/components/NavigationBar/NavigationBar.vue";
import Sidebar from "@/components/Sidebar/Sidebar.vue";

export default defineComponent({
	name: "AppLayout",
	components: {
		NavigationBar,
		Sidebar,
	},
	props: {
		Sidebar,
	},
	setup() {
		const { isMobile } = useDevice();

		const appStore = useAppStore();

		const layoutClasses = computed(() => ({
			hideNavbar: !appStore.navbar.opened,
			openNavbar: appStore.navbar.opened,
			animation: appStore.navbar.animation,
			mobile: isMobile.value,
		}));

		console.log(layoutClasses.value);

		function handleClickOutside() {
			appStore.closeNavbar(true);
		}

		return {
			layoutClasses,
			handleClickOutside,
		};
	},
});
