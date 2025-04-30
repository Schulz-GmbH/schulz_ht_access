/**
 * @file SocketService.ts
 * @brief WebSocket-Client-Service für die Anwendung.
 *
 * Detaillierte Beschreibung:
 * Dieses Modul kapselt die Verwaltung einer WebSocket-Verbindung,
 * einschließlich Verbindungsaufbau, Nachrichtenwarteschlange und Listener-Management.
 * Es ermöglicht das Senden und Empfangen von Nachrichten sowie das Hinzufügen
 * und Entfernen von Callback-Funktionen basierend auf Event- und Action-Typen.
 *
 * @author Simon Marcel Linden
 * @version 1.0.0
 * @since 1.0.0
 */

import { AppConfig } from "./_config";

/** @brief Instanz der WebSocket-Verbindung (null, wenn nicht verbunden). */
let socket: WebSocket | null = null;

/**
 * @brief Registry für eingehende Nachrichten-Listener.
 *
 * Struktur:
 * {
 *   [event: string]: {
 *     [action: string]: Function[]
 *   }
 * }
 */
let listeners: { [event: string]: { [action: string]: Function[] } } = {};

/** @brief Warteschlange für zu sendende Nachrichten, solange die Verbindung noch aufgebaut wird. */
const messageQueue: string[] = [];

/** @brief Flag, das anzeigt, ob gerade eine Verbindung aufgebaut wird. */
let isConnecting = false;

/**
 * @brief Liefert die konfigurierte WebSocket-URL.
 *
 * Die URL wird aus der zentralen Konfiguration (AppConfig) gelesen.
 *
 * @return {string} Die WebSocket-URL.
 */
function getWebSocketUrl(): string {
	return AppConfig.WS_URL;
}

/**
 * @brief Stellt sicher, dass eine WebSocket-Verbindung besteht.
 *
 * Prüft zunächst, ob der Browser online ist. Falls keine aktive Verbindung besteht
 * oder die vorhandene Verbindung geschlossen wird, wird `connect()` aufgerufen.
 *
 * @return {Promise<void>} Promise, das aufgelöst wird, wenn die Verbindung steht.
 */
async function ensureConnection(): Promise<void> {
	if (!navigator.onLine) {
		console.warn("Keine Internetverbindung – WebSocket wird nicht aufgebaut.");
		return;
	}

	if (!socket || socket.readyState === WebSocket.CLOSED || socket.readyState === WebSocket.CLOSING) {
		await connect(getWebSocketUrl());
	}
}

/**
 * @brief Verarbeitet und sendet alle Nachrichten in der Warteschlange.
 *
 * Solange die Verbindung offen ist und Nachrichten in der Queue stehen,
 * werden diese nacheinander gesendet.
 */
function processQueue(): void {
	while (messageQueue.length > 0 && socket?.readyState === WebSocket.OPEN) {
		const data = messageQueue.shift()!;
		sendMessage(data);
	}
}

/**
 * @brief Baut eine WebSocket-Verbindung zum Server auf.
 *
 * Registriert Listener für die Events `open`, `error`, `close` und `message`.
 * Messages werden beim Eintreffen geparst und an registrierte Callbacks weitergereicht.
 *
 * @param {string} url Die WebSocket-Server-URL.
 * @return {Promise<void>} Promise, das aufgelöst wird, wenn die Verbindung geöffnet ist.
 */
function connect(url?: string): Promise<void> {
	const targetUrl = url ?? getWebSocketUrl();
	console.debug("Versuche, WebSocket-Verbindung aufzubauen zu:", targetUrl);

	return new Promise((resolve, reject) => {
		if (!socket || socket.readyState !== WebSocket.OPEN) {
			isConnecting = true;
			socket = new WebSocket(targetUrl);

			socket.addEventListener("open", () => {
				console.debug("WebSocket verbunden:", targetUrl);
				isConnecting = false;
				processQueue();
				resolve();
			});

			socket.addEventListener("error", (error) => {
				console.error("WebSocket Fehler:", error);
				isConnecting = false;
				reject(error);
			});

			socket.addEventListener("close", (event) => {
				console.debug("WebSocket getrennt:", event.reason || event.code);
				socket = null;
			});

			socket.addEventListener("message", (event) => {
				// console.debug("WebSocket Nachricht empfangen:", event.data);
				try {
					const message = JSON.parse(event.data);
					const { event: evt, action, ...data } = message;
					listeners[evt]?.[action]?.forEach((cb) => cb(data));
				} catch (err) {
					console.error("Fehler beim Parsen der Nachricht:", err);
				}
			});
		} else {
			console.debug("WebSocket bereits verbunden.");
			resolve();
		}
	});
}

/**
 * @brief Trennt die bestehende WebSocket-Verbindung.
 *
 * Schließt die Verbindung und setzt die Socket-Instanz auf null zurück.
 */
function disconnect(): void {
	if (socket) {
		socket.close();
		socket = null;
		console.debug("WebSocket Verbindung geschlossen.");
	}
}

/**
 * @brief Sendet eine Nachricht über die WebSocket-Verbindung.
 *
 * Wartet bei Bedarf auf den Verbindungsaufbau, serialisiert das Datenobjekt
 * zu JSON und sendet es. Falls die Verbindung noch nicht offen ist,
 * wird die Nachricht in die Warteschlange eingefügt.
 *
 * @param {string|object} data Zu sendende Daten oder JSON-String.
 * @return {Promise<void>} Promise, das aufgelöst wird, sobald die Nachricht gesendet bzw. gequeued ist.
 */
async function sendMessage(data: string | object): Promise<void> {
	await ensureConnection();

	const message = typeof data === "string" ? data : JSON.stringify(data);
	console.log(data);
	if (socket?.readyState === WebSocket.OPEN) {
		console.debug("Sende Nachricht:", message);
		socket.send(message);
	} else {
		console.warn("Verbindung nicht bereit. Nachricht wird gequeued:", message);
		messageQueue.push(message);
	}
}

/**
 * @brief Registriert einen Listener für eingehende Nachrichten eines bestimmten Typs.
 *
 * Wartet bei Bedarf auf den Verbindungsaufbau und speichert den Callback
 * in der Listener-Registry unter `listeners[event][action]`.
 *
 * @param {string} event   Der Nachrichten-Event-Typ.
 * @param {string} action  Die Action innerhalb des Events.
 * @param {Function} callback Callback-Funktion, die aufgerufen wird, wenn eine Nachricht eintrifft.
 * @return {Promise<void>} Promise, das aufgelöst wird, wenn der Listener registriert ist.
 */
async function onMessage(event: string, action: string, callback: Function): Promise<void> {
	await ensureConnection();

	if (!listeners[event]) listeners[event] = {};
	if (!listeners[event][action]) listeners[event][action] = [];

	listeners[event][action].push(callback);
}

/**
 * @brief Entfernt einen zuvor registrierten Listener.
 *
 * Löscht den Callback aus der Listener-Registry.
 *
 * @param {string} event   Der Nachrichten-Event-Typ.
 * @param {string} action  Die Action innerhalb des Events.
 * @param {Function} callback Die zu entfernende Callback-Funktion.
 */
function removeListener(event: string, action: string, callback: Function): void {
	listeners[event]?.[action]?.splice(listeners[event][action].indexOf(callback), 1);
}

/**
 * @brief Exponierter WebSocket-Service mit allen relevanten Funktionen.
 */
export const SocketService = {
	connect,
	disconnect,
	sendMessage,
	onMessage,
	removeListener,
};
