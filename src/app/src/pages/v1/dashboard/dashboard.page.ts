import { defineComponent } from "vue";

// Layouts

// Componetns
import Widget from "../../../components/widgets/widget.vue";

/**
 * App - The root Vue component of the application.
 *
 * This component is responsible for initializing the application's state
 * and setting up the main layout.
 */
export default defineComponent({
	name: "DashboardPage",
	components: {
		Widget: Widget,
	},
	data() {
		return {};
	},

	methods: {
		handleMenuClick() {
			console.log("Menu clicked");
		},
		handleActionClick() {
			console.log("Action clicked");
		},
		customFooterAction() {
			console.log("Benutzerdefinierter Footer-Button angeklickt!");
		},
	},
	mounted() {},
});
