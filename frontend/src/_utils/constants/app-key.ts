/** Gerätetyp */
export enum DeviceEnum {
	Mobile = "Mobile",
	Desktop = "Desktop",
}

/** Layout-Modus */
export enum LayoutModeEnum {
	Top = "top",
}

// Navbar-Zustände
export const NAVBAR_OPENED = "opened";
export const NAVBAR_CLOSED = "closed";

export type NavbarOpened = typeof NAVBAR_OPENED;
export type NavbarClosed = typeof NAVBAR_CLOSED;
