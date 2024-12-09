import { SocketService } from "@/services/socket.service";
import config from "@/services/_config";

export const WlanService = {
	connect,
	disconnect,
	getStatus,
	toggleStatus,
	onStatusChange,
};

/**
 * Connects to the WebSocket server for WLAN.
 *
 * @param {string} url - The WebSocket server URL.
 * @returns {void}
 */
function connect(): void {
	SocketService.connect(`${config.socketUrl}`);
}

/**
 * Disconnects from the WebSocket server.
 *
 * @returns {void}
 */
function disconnect(): void {
	SocketService.disconnect();
}

/**
 * Fetches the current WLAN status from the server.
 *
 * @returns {Promise<boolean>} - A promise resolving to the WLAN status.
 */
async function getStatus(): Promise<boolean> {
	const response = await fetch(`${config.apiUrl}/wlan/status`);
	if (!response.ok) {
		throw new Error("Failed to fetch WLAN status.");
	}
	const data = await response.json();
	return data.wlanStatus;
}

/**
 * Toggles the WLAN status.
 *
 * @returns {Promise<void>}
 */
async function toggleStatus(): Promise<void> {
	const response = await fetch(`${config.apiUrl}/wlan/toggle`, {
		method: "POST",
		headers: {
			"Content-Type": "application/json",
		},
	});
	if (!response.ok) {
		throw new Error("Failed to toggle WLAN status.");
	}
}

/**
 * Listens for WLAN status changes via WebSocket.
 *
 * @param {Function} callback - Callback to handle the status changes.
 * @returns {void}
 */
function onStatusChange(callback: (status: boolean) => void): void {
	SocketService.onMessage("wlanStatusChanged", callback);
}
