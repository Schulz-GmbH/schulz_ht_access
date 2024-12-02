import { RouteLocationNormalized, NavigationGuardNext } from "vue-router";

import MetaTagDefinition from "../models/metatags/metatag.interface";

import { getAppName } from "../helpers/appSettings";

/**
 * MetaInfoGuard - A navigation guard for Vue Router to manage meta tags.
 *
 * This guard is responsible for setting and updating the meta tags of the document based
 * on the route being navigated to. It sets the document title and any defined meta tags
 * in the route's meta field.
 *
 * @param {RouteLocationNormalized} to - The target route being navigated to.
 * @param {RouteLocationNormalized} from - The current route being navigated from.
 * @param {NavigationGuardNext} next - A function to indicate how to proceed with the navigation.
 * @returns {void}
 *
 * @author: Simon Marcel Linden
 * @version: 1.0.0
 * @since: 1.0.0
 */
export default function MetaInfoGuard(
	to: RouteLocationNormalized,
	from: RouteLocationNormalized,
	next: NavigationGuardNext
): void {
	// Set meta tags
	const nearestWithTitle = to.matched
		.slice()
		.reverse()
		.find((r) => r.meta && r.meta.title);
	const nearestWithMeta = to.matched
		.slice()
		.reverse()
		.find((r) => r.meta && r.meta.tags);
	const previousNearestWithMeta = from.matched
		.slice()
		.reverse()
		.find((r) => r.meta && r.meta.tags);

	// If a route with a title was found, set the document (page) title to that value.
	if (nearestWithTitle) {
		document.title = `${
			nearestWithTitle.meta.title as string
		} - ${getAppName()}`;
	} else if (previousNearestWithMeta) {
		document.title = `${
			previousNearestWithMeta.meta.title as string
		} - ${getAppName()}`;
	} else {
		document.title = `${getAppName()}`;
	}

	// Remove any stale meta tags from the document using the key attribute we set below.
	Array.from(document.querySelectorAll("[data-vue-router-controlled]")).map(
		(el) => el.parentNode?.removeChild(el)
	);

	// Skip rendering meta tags if there are none.
	if (!nearestWithMeta) return next();

	const metaTags = nearestWithMeta.meta.tags;

	if (Array.isArray(metaTags)) {
		metaTags
			.map((tagDef: MetaTagDefinition) => {
				const tag = document.createElement("meta");

				Object.keys(tagDef).forEach((key) => {
					const value = tagDef[key];
					if (typeof value === "string") {
						tag.setAttribute(key, value);
					}
				});

				// We use this to track which meta tags we create so we don't interfere with other ones.
				// tag.setAttribute('data-vue-router-controlled', '');
				return tag;
			})
			.forEach((tag) => document.head.appendChild(tag));
	}

	return next();
}
