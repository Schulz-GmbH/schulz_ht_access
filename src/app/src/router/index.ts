import { createRouter, createWebHistory, RouteRecordRaw } from "vue-router";
import "vue-router";

// Import Guards and Middleware
import LogGuard from "../guards/log.guard";
// import i18nGuard from '@/guards/i18n.guard';
import MetaInfoGuard from "../guards/metainfo.guard";

/**
 * Route configurations for the Vue application.
 *
 * This array defines all the basic routes for the application, including their paths, names,
 * components, and any associated metadata. Meta tags for pages and breadcrumb information
 * are defined within the meta property of each route.
 */
const basicRoutes: Array<RouteRecordRaw> = [
	{
		path: "/dashboard",
		name: "Dashboard",
		component: () => import("@/pages/v1/dashboard/dashboard.page.vue"),
		meta: {
			title: "Dashboard",
			tags: [
				{ name: "author", value: "Simon Marcel Linden" },
				{
					name: "description",
					value: "HS - Wirelesss Access - Dashboard",
				},
				{
					name: "og:description",
					value: "HS - Wirelesss Access - Dashboard",
				},
			],
		},
	},
	{
		path: "/settings",
		name: "Settings",
		component: () => import("@/pages/v1/settings/settings.page.vue"),
		meta: {
			title: "Settings",
			tags: [
				{ name: "author", value: "Simon Marcel Linden" },
				{
					name: "description",
					value: "HS - Wirelesss Access - Settings",
				},
				{
					name: "og:description",
					value: "HS - Wirelesss Access - Settings",
				},
			],
		},
	},
];

const basisRoutes: Array<RouteRecordRaw> = [
	...basicRoutes,
	// weitere Routen hier hinzufügen
];

// Füge `basicRoutes` als Kinder von `i18nRoutes` hinzu
const routes: Array<RouteRecordRaw> = [
	{
		path: "/",
		name: "RedirectHome",
		component: () => import("@/layouts/index.layout.vue"),
		// redirect: `/${localStorage.getItem('language') || 'en'}`,
		redirect: { name: "Dashboard" },
		meta: {
			title: "Landing Page",
			tags: [
				{ name: "author", value: "Simon Marcel Linden" },
				{ name: "description", value: "Landing Page 1" },
				{
					name: "og:description",
					value: "Entwickler-Tool zur Anzeige und Überprüfung aller definierten Anwendungsrouten.",
				},
			],
		},
		children: [
			...basisRoutes,
			// Bedingte Routen nur im Development-Modus
			...(process.env.NODE_ENV === "development"
				? [
						{
							path: "routes",
							name: "RoutesPage",
							component: () =>
								import("@/pages/routes/routes.page.vue"),
							meta: {
								title: "Application Routes",
								tags: [
									{
										name: "author",
										value: "Simon Marcel Linden",
									},
									{
										name: "description",
										value: 'Diese Seite ist ausschließlich im Entwicklungsmodus zugänglich."',
									},
									{
										name: "og:description",
										value: "Entwickler-Tool zur Anzeige und Überprüfung aller definierten Anwendungsrouten.",
									},
								],
							},
						},
				  ]
				: []),
		],
	},
	{
		path: "/:catchAll(.*)",
		name: "NotFound",
		component: () => import("@/pages/v1/error/404/404.error.page.vue"),
		meta: {
			title: "Page not found - 404",
			tags: [
				{ name: "author", value: "Simon Marcel Linden" },
				{ name: "description", value: "Page not found - 404" },
				{ name: "og:description", value: "Page not found - 404" },
			],
		},
	},
];

/**
 * Creates and configures the Vue Router instance.
 *
 * This configuration sets up the router with a history mode and the defined routes.
 * It also adds global navigation guards (middlewares) for handling authentication and meta information.
 *
 * @returns {Router} The configured Vue Router instance.
 */
const router = createRouter({
	history: createWebHistory(import.meta.env.VITE_BASE_URL || "/"),
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
const globalMiddlewares = [LogGuard, MetaInfoGuard];
router.beforeEach((to, from, next) => {
	globalMiddlewares.forEach((middleware) => middleware(to, from, next));
});

export default router;
