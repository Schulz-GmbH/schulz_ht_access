import { RouteLocationNormalized, NavigationGuardNext } from "vue-router";

/**
 * Log Guard - A navigation guard for Vue Router to log router.
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
export default function LogGuard(
	_to: RouteLocationNormalized,
	_from: RouteLocationNormalized,
	next: NavigationGuardNext
) {
	// Todo: In datenbank Speichern
	next();
}
