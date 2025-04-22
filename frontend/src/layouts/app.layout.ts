import { defineComponent, defineAsyncComponent, computed, reactive, ref, type Ref, type DefineComponent } from "vue";
// Stores
import { useAppStore } from "@/store/app.store";

// composables
import { useDevice } from "@/_utils/composables/useDevice";

// Types

// Components
import NavigationBar from "@/components/NavigationBar/NavigationBar.vue";
import Sidebar from "@/components/Sidebar/Sidebar.vue";
import Footer from "@/components/Footer/Footer.vue";

type AppLayoutType = DefineComponent<{}, {}, {}>;

const AppLayout: AppLayoutType = defineComponent({
	name: "AppLayout",
	components: {
		NavigationBar,
		Sidebar,
		Footer,
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

		function handleClickOutside() {
			appStore.closeNavbar(true);
		}

		return {
			layoutClasses,
			handleClickOutside,
		};
	},
});

export default AppLayout;
