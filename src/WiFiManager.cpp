/**
 * @file WiFiManager.cpp
 * @brief Modul zur Verwaltung der WLAN-Funktionalitäten (AP + STA).
 *
 * Dieses Modul bietet Funktionen zur Verwaltung der WLAN-Konfiguration,
 * zum Laden und Speichern der STA-Konfiguration, sowie zur Initialisierung
 * und Überwachung des WLAN-Betriebs im AP+STA-Modus.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#include "WiFiManager.h"

/**
 * @brief Konstruktor für den WiFiManager.
 *
 * Initialisiert die WiFiManager-Instanz und lädt die gespeicherte STA-Konfiguration.
 */
WiFiManager::WiFiManager() : staSSID(""), staPassword("") {
	loadConfig();
	loadSavedNetworks();
}

/**
 * @brief Lädt die gespeicherte STA-Konfiguration aus den Preferences.
 *
 * Diese Methode liest die gespeicherten WLAN-Zugangsdaten (SSID und Passwort)
 * aus den Preferences und speichert sie in den entsprechenden Membervariablen.
 */
void WiFiManager::loadConfig() {
	staPreferences.begin("wifi_sta", true);  // Lese-Modus
	staSSID = staPreferences.getString("ssid", "");
	staPassword = staPreferences.getString("password", "");
	staPreferences.end();

	if (staSSID != "") {
		logger.info("Geladene STA-Konfiguration: " + staSSID);
	} else {
		logger.info("Keine gespeicherte STA-Konfiguration gefunden.");
	}
}

/**
 * @brief Speichert die aktuelle STA-Konfiguration in den Preferences.
 *
 * Diese Methode speichert die aktuellen WLAN-Zugangsdaten (SSID und Passwort)
 * in den Preferences, sodass sie bei einem Neustart wiederverwendet werden können.
 */
void WiFiManager::saveConfig() {
	staPreferences.begin("wifi_sta", false);  // Schreib-Modus
	staPreferences.putString("ssid", staSSID);
	staPreferences.putString("password", staPassword);
	staPreferences.end();
	logger.info("STA-Konfiguration gespeichert.");
}

/**
 * @brief Initialisiert das WLAN im AP+STA-Modus.
 *
 * Diese Methode konfiguriert den WiFi-Modus auf AP+STA, startet den Access Point
 * und versucht, eine Verbindung zu einem bekannten WLAN herzustellen, falls eine
 * STA-Konfiguration vorhanden ist.
 */
void WiFiManager::init() {
	// Setze den WiFi-Modus auf AP+STA
	WiFi.mode(WIFI_AP_STA);

	// Konfiguriere den Access Point mit den globalen IP-Definitionen
	if (!WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY, AP_SUBNET)) {
		logger.error("Fehler bei der Konfiguration des Access Points.");
	}
	if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
		logger.error("Access Point konnte nicht gestartet werden.");
		addStatus(STATUS_WIFI_NOT_AVAILABLE);  // Fehlerstatus setzen
	} else {
		logger.info("Access Point gestartet! IP-Adresse: " + WiFi.softAPIP().toString());
		removeStatus(STATUS_WIFI_NOT_AVAILABLE);
	}

	// Versuche, als Station zu verbinden.
	// Zuerst: Falls eine STA-Konfiguration vorhanden ist, verwende diese.
	if (staSSID != "") {
		WiFi.begin(staSSID.c_str(), staPassword.c_str());
		logger.info("Versuche, zum WLAN zu verbinden: " + staSSID);
	}
	// Falls keine explizite STA-Konfiguration vorhanden ist, aber gespeicherte Netzwerke existieren,
	// verwende das erste gespeicherte Netzwerk.
	else if (!savedNetworks.empty()) {
		staSSID = savedNetworks[0].ssid;
		staPassword = savedNetworks[0].password;
		saveConfig();  // Aktualisiere die STA-Konfiguration in den Preferences
		WiFi.begin(staSSID.c_str(), staPassword.c_str());
		logger.info("Keine STA-Konfiguration gesetzt. Verbinde mit gespeichertem Netzwerk: " + staSSID);
	} else {
		logger.warn("Keine STA-Konfiguration gesetzt und keine gespeicherten Netzwerke vorhanden.");
	}

	// Versuche, die STA-Verbindung aufzubauen:
	unsigned long startAttemptTime = millis();
	const unsigned long timeout = 10000;  // 10 Sekunden Timeout
	while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
		delay(500);
		logger.debug("Verbinde mit STA...");
	}

	if (WiFi.status() == WL_CONNECTED) {
		logger.info("STA verbunden! IP-Adresse: " + WiFi.localIP().toString());
	} else {
		logger.error("STA Verbindung fehlgeschlagen!");
	}
}

/**
 * @brief Setzt die STA-Konfiguration und speichert sie.
 *
 * Diese Methode aktualisiert die WLAN-Zugangsdaten (SSID und Passwort) und speichert
 * diese in den Preferences.
 *
 * @param ssid Die SSID des WLANs.
 * @param password Das Passwort des WLANs.
 */
void WiFiManager::setSTAConfig(const String &ssid, const String &password) {
	staSSID = ssid;
	staPassword = password;
	saveConfig();
	logger.info("STA-Konfiguration gesetzt: " + ssid);
}

/**
 * @brief Gibt die aktuelle STA-Konfiguration zurück.
 *
 * Diese Methode liefert die aktuell gespeicherten WLAN-Zugangsdaten.
 *
 * @param ssid Referenzvariable zur Rückgabe der SSID.
 * @param password Referenzvariable zur Rückgabe des Passworts.
 */
void WiFiManager::getSTAConfig(String &ssid, String &password) {
	ssid = staSSID;
	password = staPassword;
}

/**
 * @brief Fügt ein neues WLAN-Netz zur Liste der gespeicherten Netzwerke hinzu.
 *
 * Diese Methode überprüft, ob das angegebene Netzwerk bereits in der Liste vorhanden ist.
 * Falls nicht, wird es hinzugefügt.
 *
 * @param ssid Die SSID des neuen WLAN-Netzwerks.
 * @param password Das Passwort des neuen WLAN-Netzwerks.
 */
void WiFiManager::addNetwork(const String &ssid, const String &password) {
	for (const auto &net : savedNetworks) {
		if (net.ssid == ssid) {
			// Netzwerk bereits gespeichert
			return;
		}
	}
	WiFiNetwork newNet;
	newNet.ssid = ssid;
	newNet.password = password;
	savedNetworks.push_back(newNet);
	saveSavedNetworks();
}

/**
 * @brief Entfernt ein gespeichertes WLAN-Netz aus der Liste.
 *
 * Diese Methode sucht nach dem Netzwerk anhand der SSID und entfernt es, sofern vorhanden.
 *
 * @param ssid Die SSID des zu entfernenden Netzwerks.
 * @return true, falls das Netzwerk entfernt wurde; andernfalls false.
 */
bool WiFiManager::removeNetwork(const String &ssid) {
	for (auto it = savedNetworks.begin(); it != savedNetworks.end(); ++it) {
		if (it->ssid == ssid) {
			savedNetworks.erase(it);
			saveSavedNetworks();  // Persistenz aktualisieren
			logger.info("Netzwerk entfernt: " + ssid);
			return true;
		}
	}
	logger.warn("Netzwerk nicht gefunden: " + ssid);
	return false;
}

/**
 * @brief Gibt die Liste aller gespeicherten WLAN-Netzwerke zurück.
 *
 * @return Ein Vektor mit den gespeicherten Netzwerken.
 */
std::vector<WiFiNetwork> WiFiManager::getSavedNetworks() {
	return savedNetworks;
}

/**
 * @brief Lädt die gespeicherten Netzwerke aus den Preferences.
 *
 * Diese Methode liest den in den Preferences gespeicherten JSON-String aus,
 * der die Netzwerkkonfigurationen enthält, und deserialisiert diesen in ein JSON-Dokument.
 * Anschließend werden die enthaltenen Netzwerke in die interne Liste savedNetworks geladen.
 * Falls ein Fehler beim Deserialisieren auftritt, wird eine Fehlermeldung ausgegeben.
 */
void WiFiManager::loadSavedNetworks() {
	netsPreferences.begin("wifi_nets", true);  // Lese-Modus für Netzwerke
	String json = netsPreferences.getString("networks", "");
	netsPreferences.end();

	savedNetworks.clear();

	if (json.length() > 0) {
		StaticJsonDocument<1024> doc;
		DeserializationError error = deserializeJson(doc, json);
		if (error) {
			logger.error("Fehler beim Laden der Netzwerke: " + String(error.c_str()));
			return;
		}
		JsonArray array = doc.as<JsonArray>();
		for (JsonObject net : array) {
			WiFiNetwork nw;
			nw.ssid = net["ssid"].as<String>();
			nw.password = net["password"].as<String>();
			savedNetworks.push_back(nw);
		}
		// logger.info("Gespeicherte Netzwerke geladen.");
	}
}

/**
 * @brief Speichert die aktuellen Netzwerke in den Preferences als JSON.
 *
 * Diese Methode serialisiert die interne Liste der gespeicherten Netzwerke in ein JSON-Dokument.
 * Der resultierende JSON-String wird in den Preferences gespeichert, sodass alle aktuell
 * in savedNetworks enthaltenen Netzwerke persistent abgelegt sind.
 */
void WiFiManager::saveSavedNetworks() {
	StaticJsonDocument<1024> doc;
	JsonArray array = doc.to<JsonArray>();
	for (const auto &nw : savedNetworks) {
		JsonObject net = array.createNestedObject();
		net["ssid"] = nw.ssid;
		net["password"] = nw.password;
	}
	String json;
	serializeJson(doc, json);
	logger.debug("Speichere Netzwerke JSON: " + json);

	netsPreferences.begin("wifi_nets", false);  // Schreib-Modus für Netzwerke
	netsPreferences.putString("networks", json);
	netsPreferences.end();
	logger.info("Gespeicherte Netzwerke aktualisiert.");
}

/**
 * @brief Versucht, eine STA-Verbindung herzustellen.
 *
 * Zunächst wird versucht, über die gespeicherte STA-Konfiguration eine Verbindung herzustellen.
 * Scheitert dies, wird ein WLAN-Scan durchgeführt und geprüft, ob eines der gespeicherten Netzwerke
 * im Scan-Ergebnis vorhanden ist. Wird ein bekanntes Netzwerk gefunden, wird versucht, sich damit zu verbinden.
 * Bei erfolgreicher Verbindung wird die STA-Konfiguration aktualisiert.
 *
 * @return true, wenn eine Verbindung erfolgreich hergestellt wurde, sonst false.
 */
bool WiFiManager::connectSTA() {
	// Versuche, mit der gespeicherten STA-Konfiguration zu verbinden
	if (staSSID != "") {
		WiFi.begin(staSSID.c_str(), staPassword.c_str());
		logger.info("Versuche, mit der gespeicherten STA-Konfiguration zu verbinden: " + staSSID);

		unsigned long startAttemptTime = millis();
		const unsigned long timeout = 10000;  // 10 Sekunden Timeout
		while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
			delay(500);
			logger.debug("Verbinde mit STA...");
		}
		if (WiFi.status() == WL_CONNECTED) {
			logger.info("STA verbunden! IP-Adresse: " + WiFi.localIP().toString());
			return true;
		} else {
			logger.error("Verbindung mit gespeicherter STA-Konfiguration fehlgeschlagen.");
		}
	} else {
		logger.warn("Keine STA-Konfiguration vorhanden.");
	}

	// Führe einen Scan durch, um ein bekanntes Netzwerk zu finden
	logger.info("Starte Scan, um ein bekanntes Netzwerk zu finden...");
	std::vector<ScannedNetwork> scanned = scanNetworks();
	for (const auto &scannedNet : scanned) {
		// Prüfe, ob eines der gesicherten Netzwerke im Scan-Ergebnis vorhanden ist.
		for (const auto &savedNet : savedNetworks) {
			if (scannedNet.ssid == savedNet.ssid) {
				logger.info("Bekanntes Netzwerk gefunden: " + savedNet.ssid + ". Versuche Verbindung...");
				WiFi.begin(savedNet.ssid.c_str(), savedNet.password.c_str());
				unsigned long scanAttemptTime = millis();
				const unsigned long timeout = 10000;  // 10 Sekunden Timeout
				while (WiFi.status() != WL_CONNECTED && millis() - scanAttemptTime < timeout) {
					delay(500);
					logger.debug("Verbinde mit " + savedNet.ssid + "...");
				}
				if (WiFi.status() == WL_CONNECTED) {
					logger.info("STA verbunden! IP-Adresse: " + WiFi.localIP().toString());
					// Aktualisiere die STA-Konfiguration
					staSSID = savedNet.ssid;
					staPassword = savedNet.password;
					saveConfig();
					return true;
				} else {
					logger.error("Verbindung zu " + savedNet.ssid + " fehlgeschlagen.");
				}
			}
		}
	}
	logger.error("Keines der gespeicherten Netzwerke konnte verbunden werden.");
	return false;
}

/**
 * @brief Trennt die aktuelle STA-Verbindung.
 *
 * Führt einen sauberen Disconnect des STA-Modus durch und loggt die Trennung.
 */
void WiFiManager::disconnectSTA() {
	WiFi.disconnect(true);
	logger.info("STA-Verbindung getrennt.");
}

/**
 * @brief Führt einen Scan der verfügbaren WLAN-Netzwerke durch.
 *
 * Diese Methode startet einen WLAN-Scan und sammelt die Ergebnisse in einem Vektor von ScannedNetwork-Strukturen.
 *
 * @return Vektor mit den gefundenen Netzwerken.
 */
std::vector<ScannedNetwork> WiFiManager::scanNetworks() {
	std::vector<ScannedNetwork> results;
	logger.info("Starte WLAN-Scan...");
	int n = WiFi.scanNetworks();
	if (n == 0) {
		logger.info("Keine Netzwerke gefunden.");
	} else {
		for (int i = 0; i < n; i++) {
			ScannedNetwork net;
			net.ssid = WiFi.SSID(i);
			net.rssi = WiFi.RSSI(i);
			net.encryptionType = WiFi.encryptionType(i);
			net.channel = WiFi.channel(i);
			results.push_back(net);
		}
		logger.info("Scan abgeschlossen: " + String(n) + " Netzwerke gefunden.");
	}
	// Optional: WiFi.scanDelete(); // Löscht Ergebnisse, falls benötigt.
	return results;
}
