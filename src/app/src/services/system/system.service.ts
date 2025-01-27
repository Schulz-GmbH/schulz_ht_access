import { SocketService } from "@/services/_socket.service";

export const SystemService = {
	getVersion,
	updateFirmware,
};

type SystemResponse = {
	"system-version": string;
	"server-version": string;
	available: string;
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
