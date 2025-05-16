import { defineComponent } from "vue";

import { systemStatusService } from "@/_service/systemStatusService";
// Types

// Components

// Templates

export default defineComponent({
	name: "AppMain",
	components: {},
	setup() {
		systemStatusService();
		return {};
	},
});
