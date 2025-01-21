export const SocketService = {
	connect,
	disconnect,
	sendMessage,
	onMessage,
	removeListener,
};

let socket: WebSocket | null = null;
let listeners: { [key: string]: { [key: string]: Function[] } } = {};

const messageQueue: string[] = [];
let isConnecting = false;

/**
 * Connects to the WebSocket server.
 *
 * @param {string} url - The URL of the WebSocket server.
 * @returns {void}
 */
function connect(url: string): Promise<void> {
	return new Promise((resolve, reject) => {
		if (!socket || socket.readyState !== WebSocket.OPEN) {
			isConnecting = true;
			socket = new WebSocket(url);

			socket.onopen = () => {
				isConnecting = false;
				// console.log("WebSocket connected to", url);
				processQueue(); // Verarbeite alle aufgelaufenen Nachrichten
				resolve();
			};

			socket.onclose = () => {
				// console.log("WebSocket disconnected");
				socket = null;
			};

			socket.onerror = (error) => {
				// console.error("WebSocket error:", error);
				isConnecting = false;
				reject(error);
			};

			// Füge den `onmessage`-Handler hinzu
			socket.onmessage = (event) => {
				console.log("Received WebSocket message:", event.data); // Debug-Log
				try {
					const message = JSON.parse(event.data); // Parse eingehender JSON-Nachrichten
					const { event: evt, action, ...data } = message;

					// Verteile Nachrichten an registrierte Listener
					if (listeners[evt] && listeners[evt][action]) {
						listeners[evt][action].forEach((callback) => {
							callback(data);
						});
					}
				} catch (err) {
					console.error("Error parsing WebSocket message:", err);
				}
			};
		} else {
			resolve(); // Falls bereits verbunden
		}
	});
}

/**
 * Disconnects from the WebSocket server.
 *
 * @returns {void}
 */
function disconnect(): void {
	if (socket) {
		socket.close();
		socket = null;
		console.log("WebSocket disconnected");
	}
}

/**
 * Sends a message through the WebSocket in the format "event:action".
 *
 * @param {string} eventAction - The combined event and action (e.g., "wifi:get").
 * @param {Record<string, any>} data - Additional data to send (optional).
 * @returns {void}
 */
function sendMessage(data: string): void {
	if (socket && socket.readyState === WebSocket.OPEN) {
		const message = typeof data === "string" ? data : JSON.stringify(data);
		console.log("Sending message:", message); // Debug-Log
		socket.send(message);
	} else if (
		isConnecting ||
		(socket && socket.readyState === WebSocket.CONNECTING)
	) {
		const message = typeof data === "string" ? data : JSON.stringify(data);
		messageQueue.push(message);
		console.warn("WebSocket still connecting, message queued.");
	} else {
		console.error("WebSocket not connected. Unable to send message.");
	}
}

/**
 * Listens for messages from the WebSocket server.
 *
 * @param {string} event - The event name to listen for.
 * @param {Function} callback - The callback to execute when the event is received.
 * @returns {void}
 */
function onMessage(event: string, action: string, callback: Function): void {
	if (!listeners[event]) {
		listeners[event] = {};
	}
	if (!listeners[event][action]) {
		listeners[event][action] = [];
	}
	listeners[event][action].push(callback);
}

function removeListener(
	event: string,
	action: string,
	callback: Function
): void {
	if (listeners[event] && listeners[event][action]) {
		listeners[event][action] = listeners[event][action].filter(
			(cb) => cb !== callback
		);
	}
}

/**
 * Processes the queued messages once the WebSocket is connected.
 */
function processQueue(): void {
	while (
		messageQueue.length > 0 &&
		socket &&
		socket.readyState === WebSocket.OPEN
	) {
		const data = messageQueue.shift()!;
		sendMessage(data);
	}
}
