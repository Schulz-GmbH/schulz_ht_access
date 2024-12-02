#ifndef CARD_HANDLER_H
#define CARD_HANDLER_H

#include <SPI.h>
#include <SdFat.h>

// SD-Karten-Pin
extern const int chipSelect;  // Setze hier den Chip-Select Pin für dein Setup

// SdFat Instanz erstellen
extern SdFat sd;

void formatCard(SdSpiConfig spiConfig);
void writeToFile();
void readFromFile();

#endif  // RESPONSE_HANDLER_H
