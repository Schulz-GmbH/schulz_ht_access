import { SocketService } from "@/services/_socket.service";

export const SystemService = {
	getVersion,
	getApiServer,
	updateFirmware,
};

type SystemResponse = {
	"system-version": string;
	"server-version": string;
	available: string;
};

type SystemAPIResponse = {
	ip: string;
	port: string;
};

function getVersion(): Promise<SystemResponse[]> {
	return new Promise((resolve, reject) => {
		SocketService.sendMessage(
			JSON.stringify({
				command: "system",
				setting: "get",
				value: "version",
			})
		);
		SocketService.onMessage("system", "get", (response: any) => {
			if (response.status === "success") {
				resolve(response.details); // Erwartet ein Array von SystemResponse
			} else {
				reject(new Error(response.error || "Unknown error"));
			}
		});
	});
}

function getApiServer(): Promise<SystemAPIResponse> {
	return new Promise((resolve, reject) => {
		SocketService.sendMessage(
			JSON.stringify({ command: "system", setting: "get", value: "api" })
		);
		SocketService.onMessage("system", "get", (response: any) => {
			if (response.status === "success") {
				if (Array.isArray(response.details)) {
					resolve(response.details);
				} else {
					reject(new Error("Invalid response format for details"));
				}
			} else {
				reject(
					new Error(response.error || "Failed to get api information")
				);
			}
		});
	});
}

function updateFirmware(url: string): Promise<string> {
	return new Promise((resolve, reject) => {
		SocketService.sendMessage(
			JSON.stringify({ command: "system", setting: "update", value: url })
		);
		SocketService.onMessage("system", "update", (response: any) => {
			if (response.status === "success") {
				resolve(response.details);
			} else {
				reject(
					new Error(response.error || "Failed to update firmware")
				);
			}
		});
	});
}
