import { defineComponent, reactive, computed } from "vue";

import { useSystemStore } from "@/store/system/index.store";

// Components

export default defineComponent({
	name: "LogFiles",
	components: {},
	setup() {
		const systemStore = useSystemStore();

		return {
			systemStore,
		};
	},
});
