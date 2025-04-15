#ifndef HELPER_HANDLER_H
#define HELPER_HANDLER_H

#include <Arduino.h>

#define SYSTEM_VERSION "1.0.1"
#define FIRMWARE_VERSION "1.1.57"

void splitString(const String &value, char delimiter, String result[], int &resultCount);

#endif
