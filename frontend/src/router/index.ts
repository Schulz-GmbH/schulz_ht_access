import { createRouter, type RouteRecordRaw, NavigationGuard } from "vue-router";
import { routerConfig } from "@/router/config";

import { setRouteChange } from "@/_utils/composables/useRouteListener";

// Components

// Layouts
import BasisLayout from "@/layouts/basis.layout.vue";

import BasisView from "@/views/basis/basis.view.vue";
import { group } from "console";

// Pages
import Dashboard from "@/pages/dashboard/dashboard.vue";

import AboutUs from "@/pages/AboutUs/about-us.vue";
import License from "@/pages/License/license.vue";

import { terminalDefs } from "./terminal.routes";

const terminalRoutes: RouteRecordRaw[] = terminalDefs.map((def) => ({
	path: `/terminal/type-${def.suffix}`,
	name: def.name,
	component: () =>
		import(
			/* webpackChunkName: "terminal-type-${def.suffix}" */
			`@/pages/Terminal/type-${def.suffix}/terminal-type-${def.suffix}.vue`
		),
	meta: {
		title: def.title,
		icon: "fas fa-terminal",
		group: "Devices",
		hidden: false,
		dev: false,
		tags: [
			{ name: "author", value: "Simon Marcel Linden" },
			{ name: "description", value: `HS - Wireless Access - Terminal ${def.title}` },
			{ name: "og:description", value: `HS - Wireless Access - Terminal ${def.title}` },
		],
	},
}));

/**
 * Route configurations for the Vue application.
 *
 * This array defines all the basic routes for the application, including their paths, names,
 * components, and any associated metadata. Meta tags for pages and breadcrumb information
 * are defined within the meta property of each route.
 */

export const errorRoutes: Array<RouteRecordRaw> = [
	{
		path: "/403",
		component: BasisView,
		meta: {
			title: "403 - Forbidden",
			hidden: true,
		},
	},
	{
		path: "/:catchAll(.*)",
		name: "NotFound",
		component: BasisView,
		meta: {
			title: "404 - Page not found",
			hidden: true,
		},
	},
];

export const constantRoutes: Array<RouteRecordRaw> = [
	{
		path: "/",
		name: "Dashboard",
		component: Dashboard,
		meta: {
			title: "IoT Dashboard",
			icon: "fas fa-home",
			group: "",
			hidden: false,
			dev: false,
			tags: [
				{ name: "author", value: "Simon Marcel Linden" },
				{
					name: "description",
					value: "Willkommen auf der Startseite der Anwendung.",
				},
				{
					name: "og:description",
					value: "Willkommen auf der Startseite der Anwendung.",
				},
			],
		},
	},
	...terminalRoutes,
	{
		path: "/about-us",
		name: "AboutUs",
		component: AboutUs,
		meta: {
			title: "About Us",
			icon: "fa-solid fa-users",
			group: "Footer",
			hidden: true,
			dev: false,
			tags: [
				{ name: "author", value: "Simon Marcel Linden" },
				{
					name: "description",
					value: "HS - Wireless Access - About Us",
				},
				{
					name: "og:description",
					value: "HS - Wireless Access - About Us",
				},
			],
		},
	},
	{
		path: "/license",
		name: "License",
		component: License,
		meta: {
			title: "License",
			icon: "fa-solid fa-file",
			group: "Footer",
			hidden: true,
			dev: false,
			tags: [
				{ name: "author", value: "Simon Marcel Linden" },
				{
					name: "description",
					value: "HS - Wireless Access - License",
				},
				{
					name: "og:description",
					value: "HS - Wireless Access - License",
				},
			],
		},
	},
];

export const developmentRoutes: Array<RouteRecordRaw> = [
	{
		path: "routes",
		name: "RoutesPage",
		component: () => import("@/pages/routes/routes.page.vue"),
		meta: {
			title: "Application Routes",
			hidden: true,
			dev: true,
			tags: [
				{ name: "author", value: "Simon Marcel Linden" },
				{
					name: "description",
					value: "Diese Seite ist ausschließlich im Entwicklungsmodus zugänglich.",
				},
				{
					name: "og:description",
					value: "Entwickler-Tool zur Anzeige und Überprüfung aller definierten Anwendungsrouten.",
				},
			],
		},
	},
];

export const routes: Array<RouteRecordRaw> = [
	{
		path: "/",
		name: "RedirectHome",
		component: BasisLayout,
		redirect: { name: "Home" },
		children: [...constantRoutes, ...(process.env.NODE_ENV === "development" ? developmentRoutes : [])],
	},
	...errorRoutes,
];

/**
 * Creates and configures the Vue Router instance.
 *
 * This configuration sets up the router with a history mode and the defined routes.
 * It also adds global navigation guards (middlewares) for handling authentication and meta information.
 *
 * @returns {Router} The configured Vue Router instance.
 */
export const router = createRouter({
	history: routerConfig.history,
	routes,
	linkActiveClass: "active-link",
	linkExactActiveClass: "exact-active-link",
});

/**
 * Global navigation guards (middlewares).
 *
 * These are applied before each route navigation to handle tasks such as authentication
 * checks and setting meta tags for the page.
 */
// const globalMiddlewares: NavigationGuard[] = [i18nGuard, AuthenticationGuard, MetaInfoGuard];
const globalMiddlewares: NavigationGuard[] = [];
router.beforeEach((to, from, next) => {
	if (globalMiddlewares.length === 0) {
		next();
		return;
	}

	for (const middleware of globalMiddlewares) {
		middleware(to, from, next);
	}
});

router.afterEach((to) => {
	setRouteChange(to);
});

export function resetRouter() {
	try {
		router.getRoutes().forEach((route) => {
			const { name } = route;

			if (name && router.hasRoute(name)) {
				router.removeRoute(name);
			}
		});
	} catch {
		location.reload();
	}
}
