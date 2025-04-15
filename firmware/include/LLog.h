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
#include <SD.h>
#include <esp_system.h>

#include <ctime>

/**
 * @brief Singleton-Klasse zur Verwaltung von systemweitem Logging.
 *
 * Die `LLog`-Klasse bietet eine einheitliche Logging-Schnittstelle für verschiedene Log-Level.
 * Logeinträge können optional mit Zeitstempeln versehen und sowohl auf der seriellen Schnittstelle
 * als auch in eine Datei im LittleFS geschrieben werden.
 */
class LLog {
   public:
	/**
	 * @brief Gibt die Singleton-Instanz der LLog-Klasse zurück.
	 *
	 * @return Referenz auf die einzige LLog-Instanz.
	 */
	static LLog &getInstance();

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
	 * @brief Loggt eine Nachricht mit dem Log-Level DEBUG.
	 *
	 * @param message Die zu loggende Nachricht.
	 * @param newLine true = mit Zeilenumbruch, false = ohne.
	 */
	void debug(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine Nachricht mit dem Log-Level INFO.
	 */
	void info(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine Nachricht mit dem Log-Level SYSTEM.
	 */
	void system(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine Nachricht mit dem Log-Level WARNING.
	 */
	void warn(const String &message, bool newLine = true);

	/**
	 * @brief Loggt eine Nachricht mit dem Log-Level ERROR.
	 */
	void error(const String &message, bool newLine = true);

	/**
	 * @brief Aktiviert oder deaktiviert das Logging-System.
	 *
	 * @param state true aktiviert Logging, false deaktiviert es.
	 */
	static void setActive(bool state);

	/**
	 * @brief Prüft, ob das Logging aktuell aktiv ist.
	 *
	 * @return true, wenn aktiv; sonst false.
	 */
	static bool isActive();

   private:
	/// Privater Konstruktor (Singleton)
	LLog() {
	}

	// Kopierkonstruktor & Zuweisungsoperator deaktivieren
	LLog(const LLog &) = delete;
	void operator=(const LLog &) = delete;

	/**
	 * @brief Interne Methode zur Ausgabe einer Lognachricht.
	 *
	 * @param message Nachricht, die ausgegeben werden soll.
	 * @param newLine true = mit Zeilenumbruch, false = ohne.
	 * @param timestamp true = mit Zeitstempel, false = ohne.
	 */
	void logMessage(const String &message, bool newLine, bool timestamp = true);

	/**
	 * @brief Erstellt einen Zeitstempel im Format YYYY-MM-DD hh:mm:ss.
	 *
	 * @return Zeitstempel als String.
	 */
	String getTimestamp();

	// Statusvariablen
	static bool m_active;         ///< Gibt an, ob das Logging aktiv ist
	static File m_logFile;        ///< Offene Logdatei
	static String m_logFileName;  ///< Name der aktuellen Logdatei
};

/**
 * @brief Globale Referenz auf die Singleton-Instanz des Loggers.
 */
extern LLog &logger;

#endif  // LLOG_H
