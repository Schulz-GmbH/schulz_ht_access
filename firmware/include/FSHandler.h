/**
 * @file FSHandler.h
 * @brief Header-Datei zur Initialisierung und Überwachung des LittleFS-Dateisystems.
 *
 * Dieses Modul stellt Funktionen bereit, um das LittleFS-Dateisystem zu initialisieren
 * und regelmäßig auf Verfügbarkeit sowie erforderliche Verzeichnisse zu prüfen.
 * Fehlende Verzeichnisse oder ein fehlgeschlagenes Mounten führen zur Setzung entsprechender
 * Systemstatus (z. B. FS_NOT_AVAILABLE, WEBSERVER_NO_HTML_DIR, LOG_NO_DIR).
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#ifndef FSHANDLER_H
#define FSHANDLER_H

#include <LittleFS.h>

#include "LLog.h"
#include "StatusHandler.h"
#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialisiert das LittleFS-Dateisystem.
 *
 * Diese Funktion mountet das Dateisystem, erstellt ggf. die Verzeichnisse `/www/html` und `/logs`,
 * und setzt den entsprechenden Systemstatus. Sie sollte beim Systemstart aufgerufen werden.
 */
void initFS();

/**
 * @brief Führt eine zyklische Prüfung des Dateisystems und der Verzeichnisse durch.
 *
 * Diese Funktion wird regelmäßig (z. B. in `loop()` oder über einen Task) aufgerufen,
 * um zu prüfen, ob das Dateisystem weiterhin verfügbar ist und die Verzeichnisse existieren.
 * Statusflags werden entsprechend gesetzt oder entfernt.
 */
void checkFS();

#ifdef __cplusplus
}
#endif

#endif  // FSHANDLER_H
