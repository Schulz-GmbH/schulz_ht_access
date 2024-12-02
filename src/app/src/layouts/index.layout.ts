import { defineComponent } from "vue";

// import { useRouter } from "vue-router";

// Componetns

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
	components: {},
	data() {
		return {};
	},

	setup() {},
	mounted() {
		console.log("VueLayout component mounted");
	},
	beforeUnmount() {},
	methods: {},
});
