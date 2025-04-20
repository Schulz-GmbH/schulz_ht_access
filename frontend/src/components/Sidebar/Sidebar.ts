import { defineComponent, computed, onMounted } from "vue";
import { useRoute } from "vue-router";
import type { RouteRecordRaw } from "vue-router";

// Stores
import { useAppStore } from "@/store/app.store";
import { useSettingsStore } from "@/store/settings/index.store";

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
		const settingsStore = useSettingsStore();

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
			const base = "py-2.7 text-sm ease-nav-brand my-0 mx-4 flex items-center whitespace-nowrap px-4 transition-colors";
			const active = "shadow-soft-xl rounded-lg bg-white font-semibold text-slate-700";
			return route.path === r.path ? `${base} ${active}` : base;
		}

		function iconWrapperClasses(r: RouteRecordRaw) {
			const activeBg = "bg-gradient-to-tl from-blue-700 to-blue-500 text-white";
			return route.path === r.path ? activeBg : "";
		}

		console.log({ appTitle, isCollapse, route, noHiddenRoutes, sortedRoutes, linkClasses, iconWrapperClasses });
		return { appTitle, isCollapse, route, noHiddenRoutes, sortedRoutes, linkClasses, iconWrapperClasses };
	},
});
