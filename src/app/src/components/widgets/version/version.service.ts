import axios from "axios";
import config from "@/services/_config";

export const VersionService = {
	fetchVersion,
};

/**
 * Fetches the current application version from the backend.
 *
 * This function sends a GET request to the version endpoint of the backend service.
 * It handles success, error, and finally states for the request.
 *
 * @returns {Promise<{ updateAvailable: boolean }>} A promise resolving to an object indicating if an update is available.
 *
 * @author: Simon Marcel Linden
 * @version: 1.0.0
 * @since: 1.0.0
 */
async function fetchVersion() {
	return await axios
		.get(`${config.apiUrl}/version`)
		.then(handleResponse)
		.catch((error) => {
			console.error("Fehler beim Abrufen der Version:", error);
			throw new Error(
				"Fehler beim Abrufen der Version. Bitte später erneut versuchen."
			);
		});
}

/**
 * Handles the HTTP response.
 *
 * This function extracts and returns the data from the response object.
 *
 * @param {Object} response - The response object from axios.
 * @returns {T} The extracted data from the response.
 */
function handleResponse<T>(response: { data: T }): T {
	return response.data;
}
