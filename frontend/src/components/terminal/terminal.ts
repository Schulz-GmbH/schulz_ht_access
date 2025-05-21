import { defineComponent } from 'vue';
import type { DefineComponent } from 'vue';

import { useSystemStore } from '@/store/system/index';
import { useSerialIncoming } from '@/pages/terminal/composables/use-serial-incoming';

export default defineComponent({
	name: 'Terminal',
	components: {},
	setup(props) {
		const systemStore = useSystemStore();

		const { output, textareaRef } = useSerialIncoming();
		return { systemStore, output, textareaRef };
	},
});
