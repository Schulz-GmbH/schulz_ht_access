import { SocketService } from "@/services/_socket.service";

export const SerialService = {
	setBaudRate,
	disconnect,
	sendMessage,
	onMessage,
	onReceive,
	removeListener,
	removeReceiveListener,
};

/**
 * Wraps the `onMessage` method to listen for specific `serial` events.
 *
 * @param {Function} callback - The callback to execute when the event is received.
 */
function onMessage(callback: (status: string, details?: string) => void): void {
	SocketService.onMessage("serial", "send", (response: any) => {
		callback(response.status, response.details);
	});
}

function onReceive(callback: (details: string) => void): void {
	SocketService.onMessage("serial", "receive", (response: any) => {
		if (response.status === "success") {
			callback(response.details);
		} else {
			console.error("Error in received message:", response.error);
		}
	});
}

/**
 * Removes a listener for specific `serial` events.
 *
 * @param {Function} callback - The callback to remove.
 */
function removeListener(
	callback: (status: string, details?: string) => void
): void {
	SocketService.removeListener("serial", "send", callback);
}

function removeReceiveListener(callback: (details: string) => void): void {
	SocketService.removeListener("serial", "receive", callback);
}

function setBaudRate(baudRate: number): Promise<string> {
	return new Promise((resolve, reject) => {
		SocketService.sendMessage(
			JSON.stringify({
				command: "serial",
				setting: "set",
				value: baudRate.toString(),
			})
		);
		SocketService.onMessage("serial", "set", (response: any) => {
			if (response.status === "success") {
				resolve(response.details);
			} else {
				reject(new Error(response.error || "Invalid baud rate"));
			}
		});
	});
}

function disconnect(): Promise<string> {
	return new Promise((resolve, reject) => {
		SocketService.sendMessage(
			JSON.stringify({ command: "serial", setting: "disconnect" })
		);
		SocketService.onMessage("serial", "disconnect", (response: any) => {
			if (response.status === "success") {
				resolve(response.details);
			} else {
				reject(
					new Error(response.error || "No active serial connection")
				);
			}
		});
	});
}

function sendMessage(message: string): Promise<string> {
	return new Promise((resolve, reject) => {
		SocketService.sendMessage(
			JSON.stringify({
				command: "serial",
				setting: "send",
				value: message,
			})
		);
		SocketService.onMessage("serial", "send", (response: any) => {
			if (response.status === "success") {
				resolve(response.details);
			} else {
				reject(new Error(response.error || "Failed to send message"));
			}
		});
	});
}
