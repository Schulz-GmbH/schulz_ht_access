import { defineComponent, getCurrentInstance } from "vue";

export default defineComponent({
	name: "Widget",
	data() {
		return {
			localMenuOpen: this.isMenuOpen, // Arbeitskopie, die modifiziert werden kann
		};
	},
	props: {
		title: {
			type: String,
			default: "Header Title",
		},
		isMenuOpen: {
			type: Boolean,
			required: false,
			default: false,
			readOnly: false,
		},
		isNotifyOpen: {
			type: Boolean,
			required: false,
			default: false,
			readOnly: false,
		},
	},
	mounted() {
		// Verwende den Router, um das Menü zu schließen, wenn die Navigation erfolgt
		const instance = getCurrentInstance();
		const router = instance?.appContext.config.globalProperties.$router;

		if (router) {
			router.afterEach(() => {
				this.localMenuOpen = false; // Menü schließen
			});
		} else {
			console.warn("Router instance is not available.");
		}
	},
	methods: {
		isDevelopment() {
			return process.env.NODE_ENV === "development";
		},
		toggleMenu() {
			this.$emit("toggle-menu");
		},
		toggleNotify() {
			this.$emit("toggle-notify");
		},
	},
});
