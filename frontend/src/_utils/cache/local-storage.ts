import type { LayoutsConfig } from "@/layouts/_config";
import type { NavbarClosed, NavbarOpened } from "utils/constants/app-key";
import { CacheKey } from "@/_utils/constants/cache-key";

/**
 * Holt die gespeicherte Layout-Konfiguration aus dem `localStorage`.
 * @returns {LayoutsConfig | null} Die gespeicherte Konfiguration oder `null`, falls nicht vorhanden.
 */
export function getLayoutsConfig(): LayoutsConfig | null {
	const json = localStorage.getItem(CacheKey.LAYOUT_CONFIG);
	try {
		return json ? (JSON.parse(json) as LayoutsConfig) : null;
	} catch (error) {
		console.error("Fehler beim Parsen von LayoutsConfig:", error);
		return null;
	}
}

/**
 * Speichert die Layout-Konfiguration im `localStorage`.
 * @param {LayoutsConfig} settings - Die zu speichernde Layout-Konfiguration.
 */
export function setLayoutsConfig(settings: LayoutsConfig): void {
	localStorage.setItem(CacheKey.LAYOUT_CONFIG, JSON.stringify(settings));
}

/**
 * Entfernt die gespeicherte Layout-Konfiguration aus dem `localStorage`.
 */
export function removeLayoutsConfig(): void {
	localStorage.removeItem(CacheKey.LAYOUT_CONFIG);
}

/**
 * Holt den Status der Seitenleiste aus dem `localStorage`.
 * @returns {NavbarOpened | NavbarClosed | null} Der gespeicherte Status oder `null`, falls nicht vorhanden.
 */
export function getNavbarStatus(): NavbarOpened | NavbarClosed | null {
	return localStorage.getItem(CacheKey.NAVBAR_STATUS) as NavbarOpened | NavbarClosed | null;
}

/**
 * Speichert den Status der Seitenleiste im `localStorage`.
 * @param {NavbarOpened | NavbarClosed} navbarStatus - Der zu speichernde Status.
 */
export function setNavbarStatus(navbarStatus: NavbarOpened | NavbarClosed): void {
	localStorage.setItem(CacheKey.NAVBAR_STATUS, navbarStatus);
}
