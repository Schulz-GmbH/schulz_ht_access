/**
 * logBuildInfo - Utility function to log build information to the console.
 *
 * This function prints out the application's build information, including the environment
 * and version number. It is intended to be called during the application's initialization
 * phase to provide visibility into the current build being deployed.
 *
 * Environment variables `NODE_ENV` and `VUE_APP_VERSION` are used to retrieve the environment
 * and version information, respectively. This function does not return any value.
 *
 * @Author: Simon Marcel Linden
 * @Version: 1.0.0
 * @since: 1.0.0
 */
function logBuildInfo(): void {
	console.log("------------------------------------");
	console.log("Build Information:");
	console.log(`Environment: ${import.meta.env.MODE}`);
	console.log(`Version: ${import.meta.env.VITE_APP_VERSION}`);
	console.log("------------------------------------");
}

export default logBuildInfo;
