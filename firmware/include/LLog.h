/**
 * @file LLog.h
 * @brief Header-Datei für das Logging-Singleton-Modul LLog.
 *
 * Dieses Modul stellt die Deklaration der Singleton-Klasse `LLog` bereit, welche systemweites Logging auf
 * der seriellen Schnittstelle und im LittleFS-Dateisystem unterstützt. Logmeldungen werden mit einem optionalen
 * Zeitstempel versehen und in eine Logdatei geschrieben, die automatisch beim Aktivieren des Loggings erzeugt wird.
 *
 * Unterstützte Log-Level:
 * - DEBUG
 * - INFO
 * - SYSTEM
 * - WARNING
 * - ERROR
 *
 * Das Logging kann zur Laufzeit aktiviert oder deaktiviert werden. Die Klasse bietet zusätzlich einfache
 * Methoden zum Loggen mit oder ohne Zeilenumbruch, und mit/ohne Zeitstempel.
 *
 * @author Simon Marcel Linden
 * @since 1.0.0
 */

#ifndef LLOG_H
#define LLOG_H

#include <Arduino.h>
#include <LittleFS.h>

#include <ctime>
#include <vector>

/**
 * @brief Singleton-Klasse zur Verwaltung von systemweitem Logging.
 *
 * Erstellt und verwendet separate Logdateien für jedes Log-Level:
 * debug.log, info.log, system.log, warning.log, error.log
 * im Unterverzeichnis /logs/system. Logs werden zusätzlich auf der seriellen Konsole ausgegeben.
 */
class LLog {
   public:
	/**
	 * @brief Gibt die Singleton-Instanz zurück.
	 */
	static LLog &getInstance();

	static const std::vector<String> Events;
	/**
	 * @brief Gibt eine Lognachricht ohne Zeilenumbruch aus.
	 *
	 * @param message Die zu loggende Nachricht.
	 * @param timestamp true = mit Zeitstempel, false = ohne.
	 */
	void print(const String &message, bool timestamp = true);

	/**
	 * @brief Gibt eine Lognachricht mit Zeilenumbruch aus.
	 *
	 * @param message Die zu loggende Nachricht.
	 * @param timestamp true = mit Zeitstempel, false = ohne.
	 */
	void println(const String &message, bool timestamp = true);

	/**
	 * @brief Loggt eine DEBUG-Nachricht.
	 * @param message Die Nachricht.
	 */
	void debug(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine INFO-Nachricht.
	 */
	void info(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine SYSTEM-Nachricht.
	 */
	void system(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine WARNING-Nachricht.
	 */
	void warn(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine ERROR-Nachricht.
	 */
	void error(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine Socket-Nachricht.
	 */
	void socket(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine HTTP-Nachricht.
	 */
	void http(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine Nachricht in mehrere Events/Level gleichzeitig.
	 * @param events Vektor der Event-Namen ohne ".log" (z.B. {"warning","socket"}).
	 */
	void log(const std::vector<String> &events, const String &message, bool newLine = true);

	/**
	 * @brief Aktiviert oder deaktiviert das Schreiben in Dateien.
	 *        Serielle Ausgabe bleibt immer aktiv.
	 */
	static void setFileLogging(bool enabled);

	/**
	 * @brief Prüft, ob File-Logging aktiv ist.
	 */
	static bool isFileLogging();

   private:
	LLog();
	LLog(const LLog &) = delete;
	void operator=(const LLog &) = delete;

	/**
	 * @brief Interne Methode zur Ausgabe einer Lognachricht.
	 * @param level   Log-Level als Präfix (z.B. "[DEBUG]").
	 * @param message Der Log-Text.
	 * @param newLine true = mit Zeilenumbruch, false = ohne.
	 * @param timestamp true = Zeitstempel voranstellen.
	 */
	void logMessage(const char *level, const String &message, bool newLine = true, bool timestamp = true);
	void logMessage(const std::vector<String> &levels, const String &message, bool newLine = true, bool timestamp = true);

	/**
	 * @brief Erzeugt einen Zeitstempel im Format YYYY-MM-DD hh:mm:ss.
	 */
	String getTimestamp();

	static bool m_fileLogging;  ///< File-Logging an/aus

	/**
	 * @brief Schreibt eine bereits formatierte Zeile in die Logdatei.
	 *        (Nur auf Dateisystem)
	 */
	void logToFile(const String &filename, const String &entry);
};

// Convenience-Makro für globale Instanz
#define logger LLog::getInstance()

#endif  // LLOG_H
