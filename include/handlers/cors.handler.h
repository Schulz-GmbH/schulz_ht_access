#ifndef CORS_HANDLER_H
#define CORS_HANDLER_H

#include <ESPAsyncWebServer.h>

// Deklaration der Middleware-Funktionen
void addCORSHeaders(AsyncWebServerResponse *response);
void setupCORS(AsyncWebServer &server);

#endif  // CORS_HANDLER_H
