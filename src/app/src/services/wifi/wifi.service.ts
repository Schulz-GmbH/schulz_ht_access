// wifi.service.ts
import { SocketService } from "@/services/_socket.service";

export const WifiService = {
	getStatus,
	setCredentials,
	connect,
	disconnect,
	onStatusChange,
};

type WifiResponse = {
	status: "connected" | "disconnected";
	details?: string;
};

function getStatus(): Promise<WifiResponse> {
	return new Promise((resolve, reject) => {
		SocketService.sendMessage(
			JSON.stringify({ command: "wifi", setting: "status" })
		);
		SocketService.onMessage("wifi", "status", (response: any) => {
			if (response.status) {
				resolve(response);
			} else {
				reject(new Error(response.error || "Unknown error"));
			}
		});
	});
}

function setCredentials(ssid: string, password: string): Promise<string> {
	return new Promise((resolve, reject) => {
		SocketService.sendMessage(
			JSON.stringify({
				command: "wifi",
				setting: "set",
				value: `${ssid}:${password}`,
			})
		);
		SocketService.onMessage("wifi", "set", (response: any) => {
			if (response.status === "success") {
				resolve(response.details);
			} else {
				reject(
					new Error(response.error || "Invalid credentials format")
				);
			}
		});
	});
}

function connect(): Promise<string> {
	return new Promise((resolve, reject) => {
		SocketService.sendMessage(
			JSON.stringify({ command: "wifi", setting: "connect" })
		);
		SocketService.onMessage("wifi", "connect", (response: any) => {
			if (response.status === "on") {
				resolve(response.details);
			} else {
				reject(new Error(response.error || "Connection failed"));
			}
		});
	});
}

function disconnect(): Promise<string> {
	return new Promise((resolve, reject) => {
		SocketService.sendMessage(
			JSON.stringify({ command: "wifi", setting: "disconnect" })
		);
		SocketService.onMessage("wifi", "disconnect", (response: any) => {
			if (response.status === "off") {
				resolve(response.details);
			} else {
				reject(new Error(response.error || "Disconnection failed"));
			}
		});
	});
}

function onStatusChange(
	callback: (status: string, details?: string) => void
): void {
	SocketService.onMessage("wifi", "status", (response: any) => {
		callback(response.status, response.details);
	});
}
