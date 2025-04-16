// src/types/app-key.d.ts
declare module 'utils/constants/app-key' {
	export enum DeviceEnum {
		Mobile,
		Desktop,
	}

	export enum LayoutModeEnum {
		Top = 'top',
	}

	// Navbar-Zustände
	export const NAVBAR_OPENED = 'opened'
	export const NAVBAR_CLOSED = 'closed'

	export type NavbarOpened = typeof NAVBAR_OPENED
	export type NavbarClosed = typeof NAVBAR_CLOSED
}
