#include "../include/handlers/card.handler.h"

// Die Definition von sd erfolgt hier
SdFat sd;

void formatCard(SdSpiConfig spiConfig) {
	Serial.println("Formatieren der SD-Karte wird gestartet...");

	if (sd.cardBegin(spiConfig)) {
		if (sd.format()) {
			Serial.println("SD-Karte wurde erfolgreich formatiert.");
		} else {
			Serial.println("Fehler beim Formatieren der SD-Karte.");
			Serial.print("SD Fehlercode: ");
			Serial.println(sd.sdErrorCode());
			Serial.print("SD Systemfehlercode: ");
			Serial.println(sd.sdErrorData());
		}
	} else {
		Serial.println("Fehler: Die Karte konnte nicht initialisiert werden.");
		Serial.print("SD Fehlercode: ");
		Serial.println(sd.sdErrorCode());
		Serial.print("SD Systemfehlercode: ");
		Serial.println(sd.sdErrorData());
	}
}

void writeToFile() {
	Serial.println("Daten werden auf die SD-Karte geschrieben...");

	// Öffne eine Datei zum Schreiben
	SdFile myFile;
	if (myFile.open("data.txt", O_RDWR | O_CREAT | O_AT_END)) {
		Serial.println("Schreibe Daten...");
		myFile.println("Hallo, dies sind Daten, die auf der SD-Karte gespeichert werden!");
		myFile.println("ESP32 und SdFat sind großartig!");
		myFile.close();  // Datei schließen
		Serial.println("Daten erfolgreich geschrieben.");
	} else {
		Serial.println("Fehler beim Öffnen der Datei zum Schreiben.");
	}
}

void readFromFile() {
	Serial.println("Daten werden von der SD-Karte gelesen...");

	// Öffne die Datei zum Lesen
	SdFile myFile;
	if (myFile.open("data.txt", O_READ)) {
		Serial.println("Lesen der Datei...");
		char ch;
		while (myFile.available()) {
			ch = myFile.read();
			Serial.write(ch);  // Ausgabe der Datei auf dem seriellen Monitor
		}
		myFile.close();  // Datei schließen
		Serial.println("\nDaten erfolgreich gelesen.");
	} else {
		Serial.println("Fehler beim Öffnen der Datei zum Lesen.");
	}
}
