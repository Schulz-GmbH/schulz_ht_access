import { defineComponent, computed } from "vue";

// Stores
import { useAppStore } from "@/store/app.store";

// composables
import { useDevice } from "@/_utils/composables/useDevice";

// Types

// Components

export default defineComponent({
	name: "AppLayout",
	components: {},
	setup() {
		const { isMobile } = useDevice();

		const appStore = useAppStore();

		const layoutClasses = computed(() => ({
			hideNavbar: !appStore.navbar.opened,
			openNavbar: appStore.navbar.opened,
			animation: appStore.navbar.animation,
			mobile: isMobile.value,
		}));

		function handleClickOutside() {
			appStore.closeNavbar(false);
		}

		return {
			layoutClasses,
			handleClickOutside,
		};
	},
});
