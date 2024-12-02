
#include "../include/routes/wifi.routes.h"

void initWiFiRoutes(AsyncWebServer &server) {
	server.on("/wifi/status", HTTP_GET, [](AsyncWebServerRequest *request) {
		String status = getWiFiStatus();
		request->send(jsonResponse(request, 200, status));
	});

	server.on("/wifi/connect", HTTP_GET, [](AsyncWebServerRequest *request) {
		String ssid, password;
		if (readWiFiConfig(ssid, password)) {
			connectToWiFi(ssid, password);
			request->send(jsonResponse(request, 200, "WLAN verbunden."));
		} else {
			request->send(jsonResponse(request, 500, "WLAN konnte nicht verbunden werden."));
		}
	});
}
