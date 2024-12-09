import { io, Socket } from "socket.io-client";

export const SocketService = {
	connect,
	disconnect,
	sendMessage,
	onMessage,
};

let socket: Socket | null = null;

/**
 * Connects to the WebSocket server.
 *
 * @param {string} url - The URL of the WebSocket server.
 * @returns {void}
 */
function connect(url: string): void {
	if (!socket) {
		socket = io(url);
		console.log("WebSocket connected to", url);
	}
}

/**
 * Disconnects from the WebSocket server.
 *
 * @returns {void}
 */
function disconnect(): void {
	if (socket) {
		socket.disconnect();
		socket = null;
		console.log("WebSocket disconnected");
	}
}

/**
 * Sends a message through the WebSocket.
 *
 * @param {string} event - The event name.
 * @param {any} data - The data to send.
 * @returns {void}
 */
function sendMessage(event: string, data: any): void {
	if (socket) {
		socket.emit(event, data);
		console.log(`Message sent: event=${event}, data=`, data);
	}
}

/**
 * Listens for messages from the WebSocket server.
 *
 * @param {string} event - The event name to listen for.
 * @param {Function} callback - The callback to execute when the event is received.
 * @returns {void}
 */
function onMessage(event: string, callback: (data: any) => void): void {
	if (socket) {
		socket.on(event, callback);
		console.log(`Listening for event: ${event}`);
	}
}
