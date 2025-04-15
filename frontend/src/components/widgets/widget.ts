import { defineComponent } from "vue";

// Layouts

// Componetns

/**
 * Basis Widget - The root Vue component of the widget.
 *
 * This component is responsible for initializing the application's state
 * and setting up the main layout.
 */
export default defineComponent({
	name: "Widget",
	props: {
		title: {
			type: String,
			default: "Widget Header",
		},
		footerText: {
			type: String,
			default: "Last updated: just now",
		},
		actionLabel: {
			type: String,
			default: "Action",
		},
		noHeader: {
			type: Boolean,
			default: false,
		},
		noFooter: {
			type: Boolean,
			default: false,
		},
	},
	emits: ["menu-click", "action-click"],
	methods: {
		onMenuClick() {
			this.$emit("menu-click");
		},
		onActionClick() {
			this.$emit("action-click");
		},
	},
});
