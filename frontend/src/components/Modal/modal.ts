import { defineComponent, ref, watch } from "vue";
export default defineComponent({
	name: "Modal",
	props: {
		modelValue: { type: Boolean, required: true },
	},
	emits: ["update:modelValue", "confirm", "close"],
	setup(props, { emit }) {
		function close() {
			emit("update:modelValue", false);
			emit("close");
		}
		function confirm() {
			emit("confirm");
			close();
		}
		return { close, confirm };
	},
});
