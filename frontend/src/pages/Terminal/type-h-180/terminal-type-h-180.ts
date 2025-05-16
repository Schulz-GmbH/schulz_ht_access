import { defineComponent } from "vue";

import { useSerialConnection } from "@/pages/Terminal/composables/useSerialConnection";

// Components
import Terminal from "@/components/Terminal/terminal.vue";
import MobileKeyboard from "@/components/Keyboard/mobile-keyboard.vue";

export default defineComponent({
	name: "TerminalTypeH180",
	components: { Terminal, MobileKeyboard },
	setup() {
		const { connected } = useSerialConnection(112500);

		return { connected };
	},
});
