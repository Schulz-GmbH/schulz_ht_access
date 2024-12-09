import { defineComponent } from "vue";

// Layouts

// Componetns
import WlanWidget from "@/components/widgets/wlan/wlan.widget.vue";
import VersionWidget from "@/components/widgets/version/version.widget.vue";

/**
 * App - The root Vue component of the application.
 *
 * This component is responsible for initializing the application's state
 * and setting up the main layout.
 */
export default defineComponent({
	name: "DashboardPage",
	components: {
		WlanWidget: WlanWidget,
		VersionWidget: VersionWidget,
	},
	data() {
		return {};
	},
	setup() {},
	methods: {},
	mounted() {},
});
