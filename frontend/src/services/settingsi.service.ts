import axios from "axios";
import config from "@/services/_config";

export const WiFiService = {
	fetchWiFiConfig,
	submitWiFiConfig,
};

/**
 * Defines the structure of WiFi configuration data.
 */
interface WiFiData {
	ssid: string;
	password: string;
}

/**
 * Fetches the current WiFi configuration from the server.
 *
 * @returns {Promise<{ ssid: string }>} A promise resolving to the WiFi configuration object.
 */
async function fetchWiFiConfig(): Promise<{ ssid: string }> {
	try {
		const response = await axios.get(`${config.apiUrl}wifi`, {
			headers: {
				"Content-Type": "application/json",
			},
		});
		return response.data;
	} catch (error) {
		console.error("Fehler beim Abrufen der WLAN-Daten:", error);
		throw new Error("Fehler beim Abrufen der WLAN-Daten.");
	}
}

/**
 * Submits the WiFi configuration to the server.
 *
 * @param {WiFiData} wifiData - The WiFi configuration data.
 * @returns {Promise<string>} A promise resolving to the status message from the server.
 */
async function submitWiFiConfig(wifiData: WiFiData): Promise<string> {
	try {
		const response = await axios.post(`${config.apiUrl}wifi`, wifiData, {
			headers: {
				"Content-Type": "application/json",
			},
		});
		return response.data.status || "Erfolgreich gespeichert!";
	} catch (error: unknown) {
		if (error instanceof Error && axios.isAxiosError(error)) {
			console.error("Fehler beim Speichern der WLAN-Daten:", error);
			throw new Error(
				error.response?.data?.error ||
					"Fehler beim Speichern der WLAN-Daten."
			);
		} else {
			console.error(
				"Unbekannter Fehler beim Speichern der WLAN-Daten:",
				error
			);
			throw new Error("Ein unbekannter Fehler ist aufgetreten.");
		}
	}
}
