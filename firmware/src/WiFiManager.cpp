#include "WiFiManager.h"

#include <ArduinoJson.h>

/**
 * @brief Konstruktor – lädt gespeicherte Konfiguration und Netzwerke aus den Preferences.
 */
WiFiManager::WiFiManager() {
	loadConfig();
	loadNetworks();
}

/**
 * @brief Initialisiert das WLAN im Access-Point- und Station-Modus (AP+STA).
 *
 * Startet den Access Point mit statischer IP-Konfiguration. Falls aktiviert,
 * wird automatisch versucht, eine Verbindung zu einem bekannten Netzwerk herzustellen.
 * Zusätzlich wird mDNS (Bonjour/ZeroConf) aktiviert.
 */
void WiFiManager::init() {
	// Access Point
	WiFi.mode(WIFI_AP_STA);
	if (!WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY, AP_SUBNET)) {
		logger.log({"system", "error", "wifi"}, "AP-Konfiguration fehlgeschlagen");
	}
	if (WiFi.softAP(AP_SSID, AP_PASSWORD)) {
		logger.log({"system", "info", "wifi"}, "AP gestartet: " + WiFi.softAPIP().toString());
	} else {
		logger.log({"system", "error", "wifi"}, "AP konnte nicht gestartet werden");
		addStatus(WIFI_AP_NOT_AVAILABLE);
	}

	// STA automatisch verbinden, falls aktiviert und Netzwerke vorhanden
	if (enabled && !networks.empty()) {
		connectSaved();
	}

	WiFi.setHostname("hs-access");  // Groß-/Kleinschreibung unwichtig

	// 2) mDNS‐Responder starten
	if (MDNS.begin("hs-access")) {
		logger.log({"system", "info", "wifi"}, "mDNS gestartet: hs-access.local");
		// Optional: HTTP-Service ankündigen
		MDNS.addService("http", "tcp", 80);
	} else {
		logger.log({"system", "warning", "wifi"}, "mDNS konnte nicht gestartet werden");
	}
}

/**
 * @brief Versucht, eine Verbindung zu einem gespeicherten WLAN-Netzwerk herzustellen.
 *
 * Führt zunächst einen WLAN-Scan durch und vergleicht die Ergebnisse mit der Liste
 * gespeicherter Netzwerke. Verbindet sich mit dem ersten gefundenen bekannten Netzwerk.
 *
 * @return true, wenn eine Verbindung erfolgreich aufgebaut wurde; sonst false.
 */
bool WiFiManager::connectSaved() {
	auto avail = scan();
	for (const auto &nw : networks) {
		auto it = std::find_if(avail.begin(), avail.end(), [&](const Network &n) { return n.ssid == nw.ssid; });
		if (it != avail.end()) {
			return connect(nw.ssid, nw.password);
		}
	}
	logger.log({"system", "warning", "wifi"}, "Kein gespeichertes Netzwerk in Reichweite");
	return false;
}

/**
 * @brief Baut eine WLAN-Verbindung zu einem spezifischen Netzwerk auf.
 *
 * @param ssid SSID des Zielnetzwerks.
 * @param password Passwort des Zielnetzwerks.
 * @return true bei erfolgreicher Verbindung, sonst false.
 */
bool WiFiManager::connect(const String &ssid, const String &password) {
	if (!enabled) return false;

	WiFi.begin(ssid.c_str(), password.c_str());
	logger.log({"system", "info", "wifi"}, "Verbinde zu " + ssid, false);
	unsigned long start = millis();
	while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
		logger.print(".", false);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	logger.println("", false);

	if (WiFi.status() == WL_CONNECTED) {
		logger.log({"system", "info", "wifi"}, "Verbunden: " + WiFi.localIP().toString());
		if (!existsNetwork(ssid)) {
			networks.push_back({ssid, password});
			saveNetworks();
		}
		config.begin("wifi_config", false);
		config.putString("ssid", ssid);
		config.putString("password", password);
		config.end();
		return true;
	}
	logger.log({"system", "error", "wifi"}, "Verbindung zu " + ssid + " fehlgeschlagen");
	return false;
}

/**
 * @brief Trennt die bestehende WLAN-Verbindung (Station-Modus).
 *
 * @return true, wenn die Trennung erfolgreich war; sonst false.
 */
bool WiFiManager::disconnect() {
	bool ok = (WiFi.disconnect(true) == WL_DISCONNECTED);
	logger.log({"system", "info", "wifi"}, ok ? "STA-Verbindung getrennt" : "Fehler beim Trennen der STA-Verbindung");
	return ok;
}

/**
 * @brief Aktiviert den STA-Modus (Station) und speichert dies persistent.
 */
void WiFiManager::activate() {
	enabled = true;
	saveConfig();
	logger.log({"system", "info", "wifi"}, "STA aktiviert");
}

/**
 * @brief Deaktiviert den STA-Modus (Station) und speichert dies persistent.
 */
void WiFiManager::deactivate() {
	disconnect();
	enabled = false;
	saveConfig();
	logger.log({"system", "info", "wifi"}, "STA deaktiviert");
}

/**
 * @brief Fügt ein neues WLAN-Netzwerk hinzu und versucht die Verbindung.
 *
 * @param ssid SSID des Netzwerks.
 * @param password Passwort des Netzwerks.
 * @return true, wenn die Verbindung erfolgreich war, sonst false.
 */
bool WiFiManager::addNetwork(const String &ssid, const String &password) {
	// Fügt Netzwerk hinzu und verbindet direkt
	if (connect(ssid, password)) {
		return true;
	}
	return false;
}

/**
 * @brief Entfernt ein gespeichertes Netzwerk anhand der SSID.
 *
 * @param ssid SSID des zu entfernenden Netzwerks.
 * @return true, wenn das Netzwerk erfolgreich entfernt wurde; sonst false.
 */
bool WiFiManager::removeNetwork(const String &ssid) {
	// Entfernt aus gespeicherter Liste
	for (auto it = networks.begin(); it != networks.end(); ++it) {
		if (it->ssid == ssid) {
			networks.erase(it);
			saveNetworks();
			logger.log({"system", "info", "wifi"}, "Entfernt Netzwerk: " + ssid);
			return existsNetwork(ssid);
		}
	}
	logger.log({"system", "warning", "wifi"}, "Netzwerk nicht gefunden: " + ssid);
	return false;
}

/**
 * @brief Führt einen Scan nach verfügbaren WLAN-Netzwerken durch.
 *
 * @return Liste verfügbarer Netzwerke mit SSID, RSSI, Verschlüsselung und Kanal.
 */
std::vector<Network> WiFiManager::scan() const {
	std::vector<Network> results;
	int n = WiFi.scanNetworks();
	for (int i = 0; i < n; ++i) {
		results.push_back({WiFi.SSID(i), WiFi.RSSI(i), WiFi.encryptionType(i), WiFi.channel(i)});
	}
	logger.log({"system", "info", "wifi"}, String("Scan gefunden: ") + String(n) + " Netze");
	return results;
}

/**
 * @brief Gibt die Liste aller gespeicherten WLAN-Netzwerke zurück.
 *
 * @return Vektor mit gespeicherten Netzwerken (SSID + Passwort).
 */
std::vector<WiFiNetwork> WiFiManager::listNetworks() const {
	return networks;
}

/**
 * @brief Gibt die aktuell verbundene SSID zurück.
 *
 * @return SSID als String oder leerer String, wenn nicht verbunden.
 */
String WiFiManager::currentNetwork() const {
	if (WiFi.status() == WL_CONNECTED) {
		return WiFi.SSID();
	}
	return String();
}

/**
 * @brief Lädt die Einstellung für den STA-Modus aus den Preferences.
 */
void WiFiManager::loadConfig() {
	config.begin("wifi_config", true);
	enabled = config.getBool("enabled", true);
	config.end();
	logger.log({"system", "info", "wifi"}, String("STA ") + (enabled ? "aktiv" : "deaktiviert"));
}

/**
 * @brief Speichert die Einstellung für den STA-Modus in den Preferences.
 */
void WiFiManager::saveConfig() {
	config.begin("wifi_config", false);
	config.putBool("enabled", enabled);
	config.end();
}

/**
 * @brief Lädt gespeicherte WLAN-Netzwerke aus dem persistenten Speicher (JSON).
 */
void WiFiManager::loadNetworks() {
	config.begin("wifi_config", true);
	String json = config.getString("networks", "");
	config.end();
	networks.clear();
	if (!json.isEmpty()) {
		StaticJsonDocument<1024> doc;
		if (deserializeJson(doc, json) == DeserializationError::Ok) {
			for (JsonVariant v : doc.as<JsonArray>()) {
				networks.push_back({v["ssid"].as<String>(), v["password"].as<String>()});
			}
			logger.log({"system", "info", "wifi"}, "Netzwerke geladen: " + String(networks.size()));
			return;
		}
		logger.log({"system", "error", "wifi"}, "Fehler beim Parsen der Netzwerke");
	}
	logger.log({"system", "info", "wifi"}, "Keine gespeicherten Netzwerke");
}

/**
 * @brief Speichert die aktuellen WLAN-Netzwerke in den persistenten Speicher (als JSON).
 */
void WiFiManager::saveNetworks() {
	StaticJsonDocument<1024> doc;
	JsonArray arr = doc.to<JsonArray>();
	for (const auto &nw : networks) {
		JsonObject o = arr.createNestedObject();
		o["ssid"] = nw.ssid;
		o["password"] = nw.password;
	}
	String json;
	serializeJson(doc, json);
	config.begin("wifi_config", false);
	config.putString("networks", json);
	config.end();
	logger.log({"system", "info", "wifi"}, String("Netzwerke gespeichert: ") + String(networks.size()));
}

/**
 * @brief Prüft, ob ein bestimmtes Netzwerk in der gespeicherten Liste existiert.
 *
 * @param ssid SSID des Netzwerks.
 * @return true, wenn das Netzwerk vorhanden ist; sonst false.
 */
bool WiFiManager::existsNetwork(const String &ssid) const {
	return std::any_of(networks.begin(), networks.end(), [&](const WiFiNetwork &n) { return n.ssid == ssid; });
}
