import { defineComponent, computed, onMounted, ref } from "vue";

import { useRoute, useRouter, type RouteLocationMatched } from "vue-router";
import { useRouteListener } from "@/_utils/composables/useRouteListener";
import { compile } from "path-to-regexp";

// Stores

// composables

// Types

// Components

export default defineComponent({
	name: "Breadcrump",
	components: {},
	setup() {
		const route = useRoute();
		const router = useRouter();
		const { listenerRouteChange } = useRouteListener();

		// Reaktive Breadcrumb‑Liste
		const breadcrumbs = ref<RouteLocationMatched[]>([]);

		// Breadcrumbs basierend auf route.matched befüllen
		function getBreadcrumb() {
			breadcrumbs.value = route.matched.filter((item) => item.meta?.title !== undefined && item.meta?.breadcrumb !== false);
		}

		// Dynamische Routen‐Parameter in den Pfad injizieren
		function pathCompile(path: string) {
			const toPath = compile(path);
			return toPath(route.params);
		}

		// Klick‐Handler für klickbare Breadcrumbs
		function handleLink(item: RouteLocationMatched) {
			if (item.redirect) {
				router.push(item.redirect as string);
			} else {
				router.push(pathCompile(item.path));
			}
		}

		// Bei jedem Routenwechsel neu befüllen (außer bei redirect‐Routes)
		listenerRouteChange((r) => {
			if (!r.path.startsWith("/redirect/")) {
				getBreadcrumb();
			}
		}, true);

		// Einmal initial befüllen
		onMounted(getBreadcrumb);

		return {
			breadcrumbs,
			handleLink,
		};
	},
});
