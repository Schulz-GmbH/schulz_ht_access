import { defineComponent, computed, onMounted } from "vue";

// Stores

// Composables

// Types

// Components

export default defineComponent({
	name: "Hamburger",
	components: {},

	emits: ["toggleClick"],
	setup(props, { emit }) {
		function toggleClick() {
			console.log("Hamburger clicked");
			emit("toggleClick");
		}

		return {
			toggleClick,
		};
	},
});
