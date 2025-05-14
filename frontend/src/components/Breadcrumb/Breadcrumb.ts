import { defineComponent, computed, onMounted, ref } from "vue";

import { useRoute, useRouter, type RouteLocationMatched } from "vue-router";
import { useRouteListener } from "@/_utils/composables/useRouteListener";
import { compile } from "path-to-regexp";

// Stores

// composables

// Types

// Components

type Crumb = { text: string; to: string };

export default defineComponent({
	name: "Breadcrumb",
	setup() {
		const route = useRoute();
		const router = useRouter();
		const breadcrumbs = ref<Crumb[]>([]);

		// Helper: finde RouteRecordRaw nach Name
		function findRoute(name: string): RouteRecordRaw | undefined {
			return (router.getRoutes() as any[]).find((r) => r.name === name);
		}

		// Baue die Crumb-Liste durch Parent-Kette
		function buildCrumbs() {
			// 1) Sammle die Kette von aktueller Route → Eltern → …
			const chain: Crumb[] = [];
			let currentName = route.name as string | undefined;

			while (currentName) {
				const rr = findRoute(currentName);
				if (!rr || !rr.meta?.title) break;

				// 1a) Beschriftung: bei SingleFile den Dateinamen nehmen
				let text = (rr.meta as any).title as string;
				if (currentName === "SingleFile" && route.params.filename) {
					text = String(route.params.filename);
				}

				// 1b) Link mit Params substituieren
				let to = rr.path;
				Object.entries(route.params).forEach(([k, v]) => {
					to = to.replace(`:${k}`, String(v));
				});

				chain.push({ text, to });
				currentName = (rr.meta as any).breadcrumb?.parent;
			}

			// 2) Umkehren, damit der allererste Eintrag oben steht
			breadcrumbs.value = chain.reverse();
		}

		onMounted(buildCrumbs);
		// und bei jedem Routenwechsel
		router.afterEach(buildCrumbs);

		// Klick auf Crumb
		function handleLink(to: string) {
			router.push(to);
		}

		return { breadcrumbs, handleLink };
	},
});

// export default defineComponent({
// 	name: "Breadcrump",
// 	components: {},
// 	setup() {
// 		const route = useRoute();
// 		const router = useRouter();
// 		const { listenerRouteChange } = useRouteListener();

// 		// Reaktive Breadcrumb‑Liste
// 		const breadcrumbs = ref<RouteLocationMatched[]>([]);

// 		// Breadcrumbs basierend auf route.matched befüllen
// 		function getBreadcrumb() {
// 			breadcrumbs.value = route.matched.filter((item) => item.meta?.title !== undefined && item.meta?.breadcrumb !== false);
// 		}

// 		// Dynamische Routen‐Parameter in den Pfad injizieren
// 		function pathCompile(path: string) {
// 			const toPath = compile(path);
// 			return toPath(route.params);
// 		}

// 		// Klick‐Handler für klickbare Breadcrumbs
// 		function handleLink(item: RouteLocationMatched) {
// 			if (item.redirect) {
// 				router.push(item.redirect as string);
// 			} else {
// 				router.push(pathCompile(item.path));
// 			}
// 		}

// 		// Bei jedem Routenwechsel neu befüllen (außer bei redirect‐Routes)
// 		listenerRouteChange((r) => {
// 			if (!r.path.startsWith("/redirect/")) {
// 				getBreadcrumb();
// 			}
// 		}, true);

// 		// Einmal initial befüllen
// 		onMounted(getBreadcrumb);

// 		return {
// 			breadcrumbs,
// 			handleLink,
// 		};
// 	},
// });
