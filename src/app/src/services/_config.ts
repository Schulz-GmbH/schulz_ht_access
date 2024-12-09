/**
 * Configuration settings for the application.
 *
 * This object contains key configuration parameters, such as the API URL
 * that the application uses. It provides a centralized location for managing
 * such settings.
 *
 * @Author: Simon Marcel Linden
 * @Version: 1.0.0
 * @since: 1.0.0
 */

export default {
	apiUrl: `${import.meta.env.VITE_API_URL}:${import.meta.env.VITE_API_PORT}${
		import.meta.env.VITE_API_BASE_URL
	}`,
	socketUrl: `${import.meta.env.VITE_SOCKET_URL}:${
		import.meta.env.VITE_SOCKET_PORT
	}`,
};
