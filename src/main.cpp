#include <global.h>

#include "StatusHandler.h"

void setup() {
	// CPU-Frequenz auf 240 MHz setzen
	setCpuFrequencyMhz(240);

	Serial.begin(9600);
	setupStatusSystem();

	delay(10000);

	updateStatus(STATUS_READY);
}

void loop() {
}
