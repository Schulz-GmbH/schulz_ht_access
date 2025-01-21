#include "handlers/light.handler.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "handlers/response.handler.h"

void blinkTask(void *param) {
	int ledPin = *(int *)param;
	while (true) {
		digitalWrite(ledPin, HIGH);
		vTaskDelay(pdMS_TO_TICKS(500));
		digitalWrite(ledPin, LOW);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

void handleLightEvent(const String &setting, const String &value, AsyncWebSocketClient *client) {
	if (setting == "set") {
		if (value == "red:on") {
			digitalWrite(RED_LED, HIGH);
			sendResponse(client, "light", "set", "on", "red light on");
		} else if (value == "red:off") {
			digitalWrite(RED_LED, LOW);
			sendResponse(client, "light", "set", "off", "red light off");
		} else if (value == "green:on") {
			digitalWrite(GREEN_LED, HIGH);
			sendResponse(client, "light", "set", "on", "green light on");
		} else if (value == "green:off") {
			digitalWrite(GREEN_LED, LOW);
			sendResponse(client, "light", "set", "off", "green light off");
		} else if (value == "yellow:on") {
			digitalWrite(YELLOW_LED, HIGH);
			sendResponse(client, "light", "set", "on", "yellow light on");
		} else if (value == "yellow:off") {
			digitalWrite(YELLOW_LED, LOW);
			sendResponse(client, "light", "set", "off", "yellow light off");
		} else {
			sendResponse(client, "light", "error", "unknown", "", "unknown value");
		}
	} else if (setting == "blink") {
		if (value == "red") {
			xTaskCreate(blinkTask, "RedBlinkTask", 1024, (void *)&RED_LED, 1, &redBlinkTaskHandle);
			sendResponse(client, "light", "blink", "on", "red light blinking");
		} else if (value == "green") {
			xTaskCreate(blinkTask, "GreenBlinkTask", 1024, (void *)&GREEN_LED, 1, &greenBlinkTaskHandle);
			sendResponse(client, "light", "blink", "on", "green light blinking");
		} else if (value == "yellow") {
			xTaskCreate(blinkTask, "YellowBlinkTask", 1024, (void *)&YELLOW_LED, 1, &yellowBlinkTaskHandle);
			sendResponse(client, "light", "blink", "on", "yellow light blinking");
		} else {
			sendResponse(client, "light", "error", "unknown", "", "unknown blink command");
		}
	} else if (setting == "stopBlink") {
		if (value == "red" && redBlinkTaskHandle != NULL) {
			vTaskDelete(redBlinkTaskHandle);
			redBlinkTaskHandle = NULL;
			sendResponse(client, "light", "stopBlink", "off", "red light stopped blinking");
		} else if (value == "green" && greenBlinkTaskHandle != NULL) {
			vTaskDelete(greenBlinkTaskHandle);
			greenBlinkTaskHandle = NULL;
			sendResponse(client, "light", "stopBlink", "off", "green light stopped blinking");
		} else if (value == "yellow" && yellowBlinkTaskHandle != NULL) {
			vTaskDelete(yellowBlinkTaskHandle);
			yellowBlinkTaskHandle = NULL;
			sendResponse(client, "light", "stopBlink", "off", "yellow light stopped blinking");
		} else {
			sendResponse(client, "light", "error", "unknown", "", "unknown stopBlink command");
		}
	} else {
		sendResponse(client, "light", "error", "unknown", "", "unknown setting");
	}
}
