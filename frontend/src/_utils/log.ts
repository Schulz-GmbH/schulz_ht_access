/**
 * @file IndexedDBService.ts
 * @brief Verwaltung von Log-Einträgen mittels IndexedDB.
 *
 * Detaillierte Beschreibung:
 * Dieses Modul stellt Funktionen bereit, um Log-Einträge in der
 * IndexedDB unter dem Objekt-Store „logs“ zu speichern, abzurufen
 * und zu löschen. Die Datenbankversion und Store-Namen sind als
 * Konstanten definiert.
 *
 * @author Simon Marcel Linden
 * @version 1.0.0
 * @since 1.0.0
 */

/** @brief Name der IndexedDB-Datenbank. */
const DB_NAME = "AppLogs";
/** @brief Name des Objekt-Stores für Log-Einträge. */
const STORE_NAME = "logs";
/** @brief Version der Datenbank. */
const DB_VERSION = 1;

/**
 * @brief Öffnet die IndexedDB und erstellt bei Bedarf den Objekt-Store.
 *
 * Öffnet die Datenbank mit dem angegebenen Namen und der Version. Wird
 * eine neuere Version erkannt, wird im Hook `onupgradeneeded` der Store
 * „logs“ angelegt, falls dieser noch nicht existiert.
 *
 * @return {Promise<IDBDatabase>} Promise, das bei Erfolg die geöffnete Datenbank liefert.
 */
function openDB(): Promise<IDBDatabase> {
	return new Promise((resolve, reject) => {
		const request = indexedDB.open(DB_NAME, DB_VERSION);

		/**
		 * @brief Hook bei Versions-Upgrade der Datenbank.
		 *
		 * Legt den Objekt-Store „logs“ mit Auto-Increment an, falls dieser nicht existiert.
		 */
		request.onupgradeneeded = (event) => {
			const db = (event.target as IDBOpenDBRequest).result;
			if (!db.objectStoreNames.contains(STORE_NAME)) {
				db.createObjectStore(STORE_NAME, { autoIncrement: true });
			}
		};

		/** @brief Erfolg des Öffnens der Datenbank. */
		request.onsuccess = () => resolve(request.result);
		/** @brief Fehler beim Öffnen der Datenbank. */
		request.onerror = () => reject(request.error);
	});
}

/**
 * @brief Speichert einen Log-Eintrag in der IndexedDB.
 *
 * Fügt im Objekt-Store „logs“ einen Eintrag bestehend aus
 * Zeitstempel und Log-Nachricht hinzu.
 *
 * @param {string} log Die Log-Nachricht, die gespeichert werden soll.
 * @return {Promise<void>} Promise, das bei erfolgreichem Speichern aufgelöst wird.
 */
export async function saveLogToIndexedDB(log: string): Promise<void> {
	const db = await openDB();
	return new Promise((resolve, reject) => {
		const tx = db.transaction(STORE_NAME, "readwrite");
		const store = tx.objectStore(STORE_NAME);

		store.add({
			timestamp: new Date().toISOString(),
			log,
		});

		tx.oncomplete = () => resolve();
		tx.onerror = () => reject(tx.error);
		tx.onabort = () => reject(tx.error);
	});
}

/**
 * @brief Liest alle gespeicherten Log-Einträge aus der IndexedDB aus.
 *
 * Ruft alle Einträge im Objekt-Store „logs“ ab und formatiert
 * sie als Array von Strings mit Zeitstempel und Nachricht.
 *
 * @return {Promise<string[]>} Promise mit einem Array formatierter Log-Einträge.
 */
export async function getAllLogs(): Promise<string[]> {
	const db = await openDB();
	const tx = db.transaction(STORE_NAME, "readonly");
	const store = tx.objectStore(STORE_NAME);
	const request = store.getAll();

	return new Promise((resolve, reject) => {
		request.onsuccess = () => {
			const result = (request.result as any[]).map((entry) => `[${entry.timestamp}] ${entry.log}`);
			resolve(result);
		};
		request.onerror = () => reject(request.error);
	});
}

/**
 * @brief Löscht alle Log-Einträge aus der IndexedDB.
 *
 * Entfernt sämtliche Einträge aus dem Objekt-Store „logs“.
 *
 * @return {Promise<void>} Promise, das bei erfolgreichem Löschen aufgelöst wird.
 */
export async function clearLogs(): Promise<void> {
	const db = await openDB();
	return new Promise((resolve, reject) => {
		const tx = db.transaction(STORE_NAME, "readwrite");
		tx.objectStore(STORE_NAME).clear();

		tx.oncomplete = () => resolve();
		tx.onerror = () => reject(tx.error);
		tx.onabort = () => reject(tx.error);
	});
}
