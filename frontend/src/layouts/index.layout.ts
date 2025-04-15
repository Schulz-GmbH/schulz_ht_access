import { defineComponent, ref, onMounted, watch } from "vue";

// Types
import { LayoutComponent } from "@/types/layout.component";

// Componetns
import HeaderComponent from "../components/header/header.component.vue";
import NavbarComponent from "../components/navbar/navbar.component.vue";

/**
 * VueLayout - A Vue component that serves as the layout for the application.
 *
 * This component is used as a layout template for the application, containing
 * common elements such as a header and a sidebar.
 *
 * @Author: Simon Marcel Linden
 * @Version: 1.0.0
 * @since: 1.0.0
 */
export default defineComponent({
	name: "VueLayout",
	components: {
		HeaderComponent: HeaderComponent,
		NavbarComponent: NavbarComponent,
	},
	data() {
		return {
			isMenuOpen: false,
			isNotifyOpen: false,
			menuItems: [
				{
					title: "IoT Dashboard",
					icon: "fas fa-home",
					location: "/dashboard",
				},
				{
					title: "Einstellungen",
					icon: "fas fa-tools",
					location: "/settings",
				},
				{
					title: "Konsole",
					icon: "fas fa-terminal",
					location: "/console",
				},
				{ title: "Geräte", icon: "fas fa-wifi" },
				{ title: "Log-Dateien", icon: "fas fa-file" },
				{ title: "DoistDes update", icon: "fas fa-upload" },
				{ title: "Info", icon: "fas fa-info-circle" },
			],
			isMobile: window.innerWidth < 768,
		};
	},

	setup() {
		const isDevelopment = process.env.NODE_ENV === "development";

		const dynamicGridClass = ref("grid-cols-[49%_49%]"); // Standardklasse
		const childComponent = ref<LayoutComponent | null>(null); // Typisiert als LayoutComponent oder null

		// Beobachten von Änderungen an der Child-Komponente
		watch(childComponent, (newComponent) => {
			if (
				newComponent &&
				typeof newComponent.getLayoutClass === "function"
			) {
				dynamicGridClass.value = newComponent.getLayoutClass();
			}
		});

		return {
			isDevelopment,
			dynamicGridClass,
			childComponent,
		};
	},
	mounted() {
		console.log("VueLayout component mounted");
		window.addEventListener("resize", this.handleResize);
	},
	beforeUnmount() {
		window.removeEventListener("resize", this.handleResize);
	},
	methods: {
		handleToggleMenu() {
			this.isMenuOpen = !this.isMenuOpen;
		},
		handleToggleNotify() {
			this.isNotifyOpen = !this.isNotifyOpen;
		},
		handleResize() {
			this.isMobile = window.innerWidth < 768;
		},
	},
});
