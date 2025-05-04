/**
 * @file WebServerManager.cpp
 * @brief Implementierung des WebServerManagers zur Bereitstellung von Logdateien und SPA-Inhalten über HTTP.
 *
 * Dieses Modul initialisiert Routen auf einem übergebenen `AsyncWebServer`, um
 * System- und Gerätespezifische Logdateien bereitzustellen sowie eine Single Page Application (SPA) auszuliefern.
 *
 * Es werden sowohl HTML-Listen als auch Rohinhalte der Logdateien per HTTP ausgeliefert.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#include "WebServerManager.h"

#include <LittleFS.h>

#include "LLog.h"

/**
 * @brief Initialisiert die HTTP-Routen des Webservers.
 *
 * Diese Methode richtet alle erforderlichen Endpunkte für die Bereitstellung von
 * Log-Dateien, statischen Assets und der Single Page Application ein.
 *
 * @param server Referenz auf die zu konfigurierende `AsyncWebServer`-Instanz.
 */
void WebServerManager::init(AsyncWebServer &server) {
	if (!LittleFS.begin()) {
		logger.log({"system", "error", "filesystem"}, "LittleFS konnte nicht gemountet werden!");
		return;
	}
	logger.log({"system", "info", "filesystem"}, "LittleFS erfolgreich gemountet.");

	// 1) /logs → HTML-Liste aller system-Logdateien
	server.on("/logs", HTTP_GET, [this](AsyncWebServerRequest *req) { serveSystemLogList(req); });

	// 2) /logfile?level=... → einzelne Logdatei
	server.on("/logfile", HTTP_GET, [this](AsyncWebServerRequest *req) { serveSystemLog(req); });

	// 3) /logs/device?file=... → Device-Logs
	server.on("/logs/device", HTTP_GET, [this](AsyncWebServerRequest *req) { serveDeviceLog(req); });

	// 4) Assets: /css/style.css, /favicon.ico, ...
	server.serveStatic("/css", LittleFS, "/www/html/css");
	server.serveStatic("/favicon.ico", LittleFS, "/www/html/favicon.ico");

	// 5) SPA-Frontend: alles mit einem Punkt (also echte Dateien) aus /www/html, und als Fallback index.html für "Client-Routes" ohne
	// Dateierweiterung.
	auto staticHandler = server.serveStatic("/", LittleFS, "/www/html").setDefaultFile("index.html");
	staticHandler.setFilter([](AsyncWebServerRequest *req) {
		// nur dann statisch bedienen, wenn URL einen Punkt enthält (Dateiendung)
		return req->url().indexOf('.') != -1;
	});

	// 6) alle anderen Routen → index.html (Client-Routing)
	server.onNotFound([](AsyncWebServerRequest *req) { req->send(LittleFS, "/www/html/index.html", "text/html"); });
}

/**
 * @brief Antwortet mit einer HTML-Seite, die alle System-Logdateien auflistet.
 *
 * @param request Eingehende HTTP-Anfrage.
 */
void WebServerManager::serveSystemLogList(AsyncWebServerRequest *request) {
	String html = buildSystemLogListHtml();
	request->send(200, "text/html", html);
}

/**
 * @brief Generiert eine HTML-Liste aller System-Logdateien.
 *
 * Diese Methode durchsucht das Verzeichnis `/logs/system` und erstellt eine
 * formatierte HTML-Seite mit Links zu den jeweiligen Dateien.
 *
 * @return HTML-String mit Dateiliste.
 */
String WebServerManager::buildSystemLogListHtml() {
	String html =
	    "<!DOCTYPE html><html><head>"
	    "<meta charset=\"utf-8\">"
	    "<title>System-Logs</title>"
	    "<link rel=\"stylesheet\" href=\"/css/style.css\">"
	    "</head><body>"
	    "<h1>System-Logdateien</h1><ul>";

	File root = LittleFS.open("/logs/system");
	if (!root || !root.isDirectory()) {
		html += "<li><strong>Kein Log-Verzeichnis!</strong></li>";
	} else {
		File f;
		while ((f = root.openNextFile())) {
			String name = f.name();  // z.B. "/logs/system/info.log"
			int idx = name.lastIndexOf('/');
			if (idx >= 0) name = name.substring(idx + 1);
			// link auf /logfile?level=info etc.
			String level = name.substring(0, name.lastIndexOf('.'));
			String item = "<li><a href=\"/logfile?level=" + level + "\" target=\"_blank\">" + name + "</a> (" + String(f.size()) + " Bytes)</li>";
			html += item;

			f.close();
		}
	}
	html += "</ul></body></html>";
	return html;
}

/**
 * @brief Sendet den Inhalt einer System-Logdatei als HTML-Seite.
 *
 * Unterstützt Syntax-Highlighting basierend auf Log-Level ([INFO], [ERROR], etc.).
 *
 * @param request HTTP-Anfrage, die den Parameter `level` enthalten muss.
 */
void WebServerManager::serveSystemLog(AsyncWebServerRequest *request) {
	// Query-Parameter prüfen
	if (!request->hasParam("level", false)) {
		request->send(400, "text/plain", "Missing 'level'");
		return;
	}
	String lvl = request->getParam("level", false)->value();
	lvl.toLowerCase();
	if (std::find(LLog::Events.begin(), LLog::Events.end(), lvl) == LLog::Events.end()) {
		request->send(400, "text/plain", "Ungültiges Log-Level");
		return;
	}
	String path = "/logs/system/" + lvl + ".log";
	if (!LittleFS.exists(path)) {
		request->send(404, "text/plain", "Log-Datei nicht gefunden");
		return;
	}

	// Datei öffnen
	File f = LittleFS.open(path, "r");
	if (!f) {
		request->send(500, "text/plain", "Konnte Log nicht öffnen");
		return;
	}

	// HTML-Kopf mit Titel und Inline-CSS
	String html = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="utf-8">
  <title>Log-File: )rawliteral" +
	              lvl + R"rawliteral(</title>
	<link rel="stylesheet" href="css/style.css">
</head>
<body>
  <h1>Log-File: )rawliteral" +
	              lvl + R"rawliteral(</h1>
  <pre>
)rawliteral";

	// Zeile für Zeile einlesen und spannen‐weise einfärben
	while (f.available()) {
		String line = f.readStringUntil('\n');
		// Prüfen, ob "[INFO]" / "[ERROR]" / "[WARNING]" im String vorkommt
		if (line.indexOf("[INFO]") != -1) {
			html += "<span class='info'>" + line + "</span>\n";
		} else if (line.indexOf("[ERROR]") != -1) {
			html += "<span class='error'>" + line + "</span>\n";
		} else if (line.indexOf("[WARNING]") != -1) {
			html += "<span class='warning'>" + line + "</span>\n";
		} else if (line.indexOf("[LLOG]") != -1) {
			html += "<span class='warning'>" + line + "</span>\n";
		} else {
			html += line + "\n";
		}
	}
	f.close();

	// HTML-Fuß
	html += R"rawliteral(
  </pre>
</body>
</html>
)rawliteral";

	// Abschicken als HTML
	request->send(200, "text/html", html);
}

/**
 * @brief Sendet den Inhalt einer Gerätelogdatei (`/logs/device/<filename>`) im Klartext.
 *
 * @param request HTTP-Anfrage, die den Parameter `file` enthalten muss.
 */
void WebServerManager::serveDeviceLog(AsyncWebServerRequest *request) {
	if (!request->hasParam("file", false)) {
		request->send(400, "text/plain", "Missing 'file' parameter");
		return;
	}
	String fn = request->getParam("file", false)->value();
	if (fn.indexOf('/') != -1 || fn.indexOf("..") != -1) {
		request->send(400, "text/plain", "Invalid filename");
		return;
	}
	String path = "/logs/device/" + fn;
	if (!LittleFS.exists(path)) {
		request->send(404, "text/plain", "File not found");
		return;
	}
	auto res = request->beginResponse(LittleFS, path, "text/plain");
	res->addHeader("Access-Control-Allow-Origin", "*");
	request->send(res);
}
