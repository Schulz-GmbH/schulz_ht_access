#include "handlers/system.handler.h"

#include "handlers/response.handler.h"

void handleSystemEvent(const String &setting, const String &value, AsyncWebSocketClient *client) {
	if (setting == "get" && value == "version") {
		sendResponse(client, "system", "get", "success", "Firmware-Version: 1.0.0");
	} else if (setting == "update") {
		// Angenommen, ein Update-Prozess wird hier gestartet
		sendResponse(client, "system", "update", "success", "Update gestartet mit URL: " + value);
	} else {
		sendResponse(client, "system", "error", "unknown", "", "unknown system setting");
	}
}
