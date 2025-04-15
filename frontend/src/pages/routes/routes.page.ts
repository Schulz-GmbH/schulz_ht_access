import { defineComponent } from "vue";
import { RouteRecordRaw } from "vue-router";

import router from "../../router";
// Stelle sicher, dass der Pfad zu deiner Router-Instanz korrekt ist

// Components

export default defineComponent({
	name: "RoutesdPage",
	components: {},
	computed: {
		routes() {
			return router
				.getRoutes()
				.map((route: RouteRecordRaw) => ({
					name: route.name as string,
					path: route.path,
				}))
				.sort();
		},
	},
});
