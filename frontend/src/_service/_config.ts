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

export const AppConfig = {
	WS_URL: import.meta.env.VITE_WS_URL || "ws://localhost:80",
};
