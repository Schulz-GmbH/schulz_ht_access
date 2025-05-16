import { defineComponent, computed, ref, watch, nextTick, provide, onMounted, onUnmounted } from "vue";

import { useSystemStore } from "@/store/system/index.store";
import { useSerialIncoming } from "@/pages/Terminal/composables/useSerialIncoming";

// Components

export default defineComponent({
	name: "Terminal",
	components: {},
	setup(props) {
		const systemStore = useSystemStore();

		const { output, textareaRef } = useSerialIncoming();
		return { systemStore, output, textareaRef };
	},
});
