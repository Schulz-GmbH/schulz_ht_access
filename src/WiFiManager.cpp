/**
 * @file WiFiManager.cpp
 * @brief Modul zur Verwaltung der WLAN-Funktionalitäten (AP + STA).
 *
 * Dieses Modul bietet Funktionen zur Verwaltung der WLAN-Konfiguration,
 * einschließlich Laden und Speichern der STA-Konfiguration und gespeicherter Netzwerke.
 * Es ermöglicht das Verbinden zu WLAN-Netzwerken, Verwalten gespeicherter Netzwerke,
 * das Durchführen von Scans nach verfügbaren Netzwerken und unterstützt den Betrieb
 * im kombinierten Access-Point- und Station-Modus (AP+STA).
 *
 * WLAN-Konfigurationen und gespeicherte Netzwerke werden persistent gespeichert
 * und bei Neustart automatisch geladen.
 *
 * Außerdem ermöglicht das Modul das Aktivieren und Deaktivieren der WLAN-Funktionalität
 * über eine gespeicherte Einstellung (Preference).
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#include "WiFiManager.h"

/**
 * @brief Konstruktor für den WiFiManager.
 *
 * Initialisiert die Instanz und lädt sowohl die gespeicherte STA-Konfiguration
 * als auch die gespeicherten Netzwerke aus dem persistenten Speicher.
 */
WiFiManager::WiFiManager() {
	this->loadConfig();
	this->loadNetworks();
}

/**
 * @brief Initialisiert das WLAN im AP+STA-Modus.
 *
 * Diese Methode konfiguriert den WiFi-Modus als Access Point und Station (AP+STA).
 * Sie startet den Access Point mit vordefinierten Zugangsdaten.
 * Wenn WLAN aktiviert ist und gespeicherte Netzwerkdaten vorhanden sind,
 * versucht sie automatisch, eine Verbindung herzustellen.
 */
void WiFiManager::init() {
	// Wenn WLAN deaktiviert ist, wird keine STA-Verbindung aufgebaut.
	if (!this->enabled) {
		logger.warn("[WiFi] WLAN ist per Preference deaktiviert.");
		WiFi.mode(WIFI_AP);
	} else {
		// Setze den WiFi-Modus auf AP+STA
		WiFi.mode(WIFI_AP_STA);
	}

	if (!WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY, AP_SUBNET)) {
		logger.error("[WiFi] Fehler bei der Konfiguration des Access Points.");
	}
	if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
		logger.error("[WiFi] Access Point konnte nicht gestartet werden.");
		addStatus(WIFI_AP_NOT_AVAILABLE);  // Fehlerstatus setzen
	} else {
		logger.info("[WiFi] Access Point gestartet! IP-Adresse: " + WiFi.softAPIP().toString());
		removeStatus(WIFI_AP_NOT_AVAILABLE);
	}

	if (this->ssid == "" || this->password == "") {
		return;
	}

	if (this->networks.empty()) {
		return;
	}

	if (this->connect() == WL_CONNECTED) {
		logger.info("[WiFi] Wifi mit " + this->ssid + " verbunden");
		return;
	}
}

/**
 * @brief Lädt die WLAN-Konfiguration aus den Preferences.
 *
 * Lädt die gespeicherten Zugangsdaten (SSID und Passwort) sowie den Aktivierungsstatus aus dem persistenten Speicher.
 */
void WiFiManager::loadConfig() {
	config.begin("wifi_config", true);
	this->ssid = config.getString("ssid", "");
	this->password = config.getString("password", "");
	this->enabled = config.getBool("enabled", true);
	config.end();

	if (this->ssid != "") {
		logger.info("[WiFi] Geladene WiFi-Konfiguration: " + this->ssid);
	} else {
		logger.info("[WiFi] Keine gespeicherte STA-Konfiguration gefunden.");
	}
}

/**
 * @brief Speichert die WLAN-Konfiguration in den Preferences.
 *
 * Speichert die aktuellen Zugangsdaten (SSID und Passwort) und den Aktivierungsstatus im persistenten Speicher.
 */
void WiFiManager::saveConfig() {
	config.begin("wifi_config", false);  // Schreib-Modus
	config.putString("ssid", this->ssid);
	config.putString("password", this->password);
	config.putBool("enabled", this->enabled);
	config.end();
}

/**
 * @brief Lädt gespeicherte WLAN-Netzwerke aus den Preferences.
 *
 * Lädt eine persistierte Liste gespeicherter WLAN-Netzwerke im JSON-Format aus dem persistenten Speicher.
 */
void WiFiManager::loadNetworks() {
	config.begin("wifi_config", true);
	String json = config.getString("networks", "");
	config.end();

	this->networks.clear();

	if (json.length() > 0) {
		StaticJsonDocument<1024> doc;
		DeserializationError error = deserializeJson(doc, json);
		if (error) {
			logger.error("[WiFi] Fehler beim Laden der Netzwerke: " + String(error.c_str()));
			return;
		}
		JsonArray array = doc.as<JsonArray>();
		for (JsonObject net : array) {
			WiFiNetwork nw;
			nw.ssid = net["ssid"].as<String>();
			nw.password = net["password"].as<String>();
			this->networks.push_back(nw);
		}
		logger.info("[WiFi] Gespeicherte Netzwerke geladen.");
	} else {
		logger.info("[WiFi] Keine gespeicherten Netzwerke gefunden.");
	}
}

/**
 * @brief Speichert die aktuellen WLAN-Netzwerke in den Preferences.
 *
 * Serialisiert die interne Liste der Netzwerke in JSON und speichert sie im persistenten Speicher.
 */
void WiFiManager::saveNetworks() {
	StaticJsonDocument<1024> doc;
	JsonArray array = doc.to<JsonArray>();
	for (const auto &nw : this->networks) {
		JsonObject net = array.createNestedObject();
		net["ssid"] = nw.ssid;
		net["password"] = nw.password;
	}
	String json;
	serializeJson(doc, json);
	logger.debug("[WiFi] Speichere Netzwerke JSON: " + json);

	config.begin("wifi_config", false);  // Schreib-Modus für Netzwerke
	config.putString("networks", json);
	config.end();
	logger.info("[WiFi] Gespeicherte Netzwerke aktualisiert.");
};

/**
 * @brief Gibt die aktuell gespeicherte STA-SSID zurück.
 *
 * Diese Methode gibt die aktuell konfigurierte SSID zurück, die für den Station-Modus (STA)
 * gespeichert wurde.
 *
 * @param ssid Referenzvariable, in der die SSID gespeichert wird.
 */
void WiFiManager::getNetwork(String &ssid) {
	ssid = this->ssid;
}

/**
 * @brief Versucht, sich mit gespeicherten WLAN-Netzwerken zu verbinden.
 *
 * Führt einen WLAN-Scan durch und versucht, mit gespeicherten Netzwerken, die aktuell verfügbar sind, eine Verbindung aufzubauen.
 *
 * @return WLAN-Verbindungsstatus nach dem Verbindungsversuch.
 */
wl_status_t WiFiManager::connect() {
	std::vector<Network> availableNetworks = this->scan();
	int status = WiFi.status();

	if (availableNetworks.empty()) {
		logger.warn("[WiFi] Keine Netzwerke gefunden.");
		return WL_DISCONNECTED;
	}

	// Iteriere über alle gespeicherten Netzwerke
	for (const auto &net : this->networks) {
		for (const auto &scan : availableNetworks) {
			if (net.ssid == scan.ssid) {
				WiFi.begin(net.ssid.c_str(), net.password.c_str());
				logger.info("[WiFi] Versuche, mit dem Netzwerk " + net.ssid + " zu verbinden.", false);
				unsigned long attemptTime = millis();
				const unsigned long timeout = 10000;  // 10 Sekunden Timeout
				while (WiFi.status() != WL_CONNECTED && millis() - attemptTime < timeout) {
					vTaskDelay(pdMS_TO_TICKS(500));
					logger.print(".", false);
				}
				logger.println("", false);

				if (WiFi.status() == WL_CONNECTED) {
					logger.info("[WiFi] Wifi verbunden! IP-Adresse: " + WiFi.localIP().toString());
					this->ssid = net.ssid;
					this->password = net.password;
					saveConfig();
					return WiFi.status();
				}
			}
		}
	}
	return WiFi.status();
};

/**
 * @brief Verbindet explizit mit einem angegebenen WLAN-Netzwerk und speichert es.
 *
 * @param ssid SSID des Netzwerks.
 * @param password Passwort des Netzwerks.
 * @return WLAN-Verbindungsstatus nach dem Versuch.
 */
wl_status_t WiFiManager::connect(const String ssid, const String password) {
	this->addNetwork(ssid, password);

	WiFi.begin(ssid.c_str(), password.c_str());
	logger.info("[WiFi] Versuche, mit dem Netzwerk " + ssid + " zu verbinden.", false);
	unsigned long attemptTime = millis();
	const unsigned long timeout = 10000;  // 10 Sekunden Timeout
	while (WiFi.status() != WL_CONNECTED && millis() - attemptTime < timeout) {
		vTaskDelay(pdMS_TO_TICKS(500));
		logger.print(".", false);
	}
	logger.println("", false);

	if (WiFi.status() == WL_CONNECTED) {
		logger.info("[WiFi] Wifi verbunden! IP-Adresse: " + WiFi.localIP().toString());
		this->ssid = ssid;
		this->password = password;
		saveConfig();
	} else {
		logger.error("[WiFi] Verbindung zu " + ssid + " fehlgeschlagen.");
	}
	return WiFi.status();
};

/**
 * @brief Trennt die aktuelle WLAN-Verbindung.
 *
 * @return true, wenn erfolgreich getrennt, sonst false.
 */
bool WiFiManager::disconnect() {
	return WiFi.disconnect(true) ? (logger.info("[WiFi] STA-Verbindung getrennt."), true)
	                             : (logger.error("[WiFi] Fehler beim Trennen der Verbindung."), false);
}

/**
 * @brief Scannt nach verfügbaren WLAN-Netzwerken.
 *
 * @return Liste gefundener Netzwerke mit SSID, RSSI, Verschlüsselung und Kanal.
 */
std::vector<Network> WiFiManager::scan() {
	std::vector<Network> results;
	logger.info("[WiFi] Starte WLAN-Scan...");
	int n = WiFi.scanNetworks();
	if (n > 0) {
		for (int i = 0; i < n; i++) {
			Network net;
			net.ssid = WiFi.SSID(i);
			net.rssi = WiFi.RSSI(i);
			net.encryptionType = WiFi.encryptionType(i);
			net.channel = WiFi.channel(i);
			results.push_back(net);
		}
	}
	return results;
}

/**
 * @brief Fügt ein WLAN-Netzwerk zur gespeicherten Liste hinzu.
 *
 * @param ssid SSID des Netzwerks.
 * @param password Passwort des Netzwerks.
 */
void WiFiManager::addNetwork(const String ssid, const String password) {
	if (!this->existsNetwork(ssid)) {
		WiFiNetwork nw;
		nw.ssid = ssid;
		nw.password = password;
		this->networks.push_back(nw);
		this->saveNetworks();
	}
}

/**
 * @brief Entfernt ein gespeichertes WLAN-Netzwerk anhand der SSID.
 *
 * Sucht nach der angegebenen SSID in der Liste der gespeicherten Netzwerke.
 * Wird das Netzwerk gefunden, entfernt die Methode das Netzwerk aus der Liste
 * und aktualisiert anschließend die persistierte Liste.
 *
 * @param ssid Die SSID des zu entfernenden WLAN-Netzwerks.
 * @return true, falls das Netzwerk erfolgreich entfernt wurde; andernfalls false.
 */
bool WiFiManager::removeNetwork(const String ssid) {
	for (size_t i = 0; i < this->networks.size(); i++) {
		if (this->networks[i].ssid == ssid) {
			this->networks.erase(this->networks.begin() + i);
			this->saveNetworks();
			return true;
		}
	}
	return false;
}

/**
 * @brief Gibt eine Liste aller gespeicherten WLAN-Netzwerke zurück.
 *
 * Diese Methode erstellt und liefert eine Kopie der aktuell gespeicherten Netzwerke
 * als Vektor zurück. Jedes Element enthält die SSID und das zugehörige Passwort.
 *
 * @return Vektor mit gespeicherten WLAN-Netzwerken.
 */
std::vector<WiFiNetwork> WiFiManager::listNetwork() {
	std::vector<WiFiNetwork> networks;
	for (size_t i = 0; i < this->networks.size(); i++) {
		networks.push_back(this->networks[i]);
	}

	return networks;
}

/**
 * @brief Prüft, ob ein WLAN-Netzwerk bereits in der gespeicherten Liste existiert.
 *
 * Durchsucht die Liste gespeicherter WLAN-Netzwerke nach einer bestimmten SSID.
 *
 * @param ssid Die SSID, die geprüft werden soll.
 * @return true, falls das Netzwerk existiert; andernfalls false.
 */
bool WiFiManager::existsNetwork(const String &ssid) {
	for (const auto &net : this->networks) {
		if (net.ssid == ssid) {
			return true;
		}
	}
	return false;
}
