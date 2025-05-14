import { defineComponent, computed, onMounted } from "vue";
import { useRoute } from "vue-router";
import type { RouteRecordRaw } from "vue-router";

// Stores
import { useAppStore } from "@/store/app.store";
import { useSystemStore } from "@/store/system/index.store";

// composables

// Types

// Interfaces
export interface RouteGroup {
	group: string;
	items: RouteRecordRaw[];
}

// Components
import Logo from "@/components/logo/logo.components.vue";

export default defineComponent({
	name: "Sidebar",
	components: {
		Logo,
	},
	setup() {
		const appStore = useAppStore();
		const settingsStore = useSystemStore();

		const route = useRoute();
		const noHiddenRoutes = computed(() => settingsStore.routes.filter((item) => !item.meta?.hidden && item.meta?.dev !== true));
		const filteredRoutes = computed<RouteRecordRaw[]>(() => settingsStore.routes.filter((r) => !r.meta?.hidden && !r.meta?.dev));

		const sortedRoutes = computed<RouteGroup[]>(() => {
			const map = new Map<string, RouteRecordRaw[]>();

			for (const r of filteredRoutes.value) {
				const grpRaw = r.meta?.group;
				const grp = typeof grpRaw === "string" ? grpRaw : "";
				if (!map.has(grp)) map.set(grp, []);
				map.get(grp)!.push(r);
			}

			const result: RouteGroup[] = [];

			if (map.has("")) {
				result.push({ group: "", items: map.get("")! });
			}

			for (const [group, items] of map.entries()) {
				if (group === "") continue;
				result.push({ group, items });
			}

			return result;
		});

		const appTitle = import.meta.env.VITE_APP_TITLE;

		const isCollapse = computed(() => !appStore.navbar.opened);

		function linkClasses(r: RouteRecordRaw) {
			const base = "bg-white ease-soft-in-out flex items-center mx-4 my-0 px-4 py-2  text-sm transition-all whitespace-nowrap";
			const active = "active xl:shadow-soft-xl rounded-lg font-semibold text-slate-700";
			return route.path === r.path ? `${base} ${active}` : base;
		}

		function iconWrapperClasses(r: RouteRecordRaw) {
			const base = "shadow-soft-2xl  mr-2 flex h-8 w-8 items-center justify-center rounded-lg bg-white";
			const active = "stroke-none shadow-soft-sm bg-gradient-to-tl from-blue-700 to-blue-500 bg-center p-2.5 text-center text-white";
			return route.path === r.path ? `${base} ${active}` : base;
		}

		return { appTitle, isCollapse, route, noHiddenRoutes, sortedRoutes, linkClasses, iconWrapperClasses };
	},
});
