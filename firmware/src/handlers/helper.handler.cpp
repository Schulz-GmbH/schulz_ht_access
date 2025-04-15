#include "handlers/helper.handler.h"

void splitString(const String &value, char delimiter, String result[], int &resultCount) {
	resultCount = 0;  // Anzahl der gefundenen Teile zurücksetzen
	int startIndex = 0;
	int delimiterIndex = value.indexOf(delimiter);

	while (delimiterIndex != -1) {
		// Teilstring extrahieren und in das Array einfügen
		result[resultCount++] = value.substring(startIndex, delimiterIndex);
		startIndex = delimiterIndex + 1;  // Startindex für den nächsten Teil
		delimiterIndex = value.indexOf(delimiter, startIndex);
	}

	// Letzten Teil hinzufügen (nach dem letzten Delimiter)
	if (startIndex < value.length()) {
		result[resultCount++] = value.substring(startIndex);
	}
}
