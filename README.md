# ESP32 IoT Access Point Webinterface

## Projektbeschreibung

Dieses Projekt ist ein Webinterface für den ESP32, welches als Access Point (AP) fungiert, um die Konfiguration und Verwaltung von Netzwerk- und Dateisystemfunktionen zu ermöglichen. Es dient dazu, verschiedene Netzwerkeinstellungen bequem über eine Weboberfläche vorzunehmen und Dateien im eingebauten Dateisystem des ESP32 zu verwalten.

Das Projekt verwendet die folgenden Technologien und Bibliotheken:

- **ESPAsyncWebServer**: Für die Bereitstellung des Webservers, der die HTTP-Endpunkte bereitstellt.
- **WebSockets**: Zur Echtzeit-Kommunikation zwischen dem ESP32 und dem Frontend, um z.B. den WLAN-Status live zu aktualisieren.
- **LittleFS**: Als Dateisystem zur Speicherung und Verwaltung von Konfigurationsdateien und weiteren Ressourcen direkt auf dem ESP32.
- **mDNS**: Ermöglicht das einfache Auffinden des ESP32 im Netzwerk unter einem lesbaren Hostnamen ("esp32.local").

### Funktionen

1. **WLAN-Konfiguration und Verwaltung**

   - Erstellung eines Access Points zur lokalen Verbindung.
   - Speicherung und Verwaltung der WLAN-Anmeldeinformationen im Dateisystem.
   - Möglichkeit, zwischen Access Point (AP) und Station (STA) Modus zu wechseln.
   - Anzeige des aktuellen WLAN-Status.

2. **Dateisystemverwaltung**

   - Auflisten der Dateien, die im LittleFS gespeichert sind.
   - Löschen von Dateien über eine Weboberfläche.

3. **Firmware-Updates**

   - Das System kann über das Webinterface neue Firmware oder Webinterface-Updates herunterladen und anwenden.
   - Die Updates werden in einem separaten Thread ausgeführt, um eine möglichst unterbrechungsfreie Benutzererfahrung zu bieten.

4. **WebSocket-Schnittstelle**

   - Für Funktionen wie das Echtzeit-Übermitteln des WLAN-Status an das Frontend wird eine WebSocket-Verbindung genutzt.
   - Damit ist sichergestellt, dass Änderungen in den Netzwerkeinstellungen sofort im Webinterface sichtbar sind.

5. **Serielle Schnittstelle für externe Geräte**

   - Das Webinterface ermöglicht das Auslesen von Daten eines externen Gerätes über die serielle Schnittstelle.
   - Benutzer können über das Webinterface Daten oder Eingaben an das externe Gerät senden.
   - Zusätzlich kann das externe Gerät über die serielle Schnittstelle aktualisiert werden.

### Projektstruktur

- `main.cpp`: Startpunkt des Programms. Initialisiert den Webserver, das Dateisystem und den Access Point.
- `routes/`
  - **`file_routes.cpp`**: Stellt Endpunkte zur Dateiverwaltung bereit (z.B. Auflisten und Löschen von Dateien).
  - **`wifi_routes.cpp`**: Beinhaltet die Endpunkte zur WLAN-Konfiguration und Verwaltung.
- `handlers/`
  - **`response_handler.cpp`**: Enthält die Funktion, um konsistente JSON-Antworten auf HTTP-Anfragen zurückzugeben.
  - **`wifi_handler.cpp`**: Verwaltet die WLAN-Verbindungen und den entsprechenden Status.
  - **`update_handler.cpp`**: Behandelt das Firmware-Update und das Herunterladen sowie Entpacken von Dateien.

### Verwendung

1. **Clone das Repository**
   ```bash
   git clone https://github.com/username/esp32-iot-access-point.git
   cd esp32-iot-access-point
   ```
3. **Bibliotheken installieren** Stelle sicher, dass die notwendigen Bibliotheken (ESPAsyncWebServer, LittleFS, etc.) in der Arduino IDE installiert sind.
4. **Upload auf den ESP32** Lade den Code mit der Arduino IDE oder PlatformIO auf den ESP32 hoch.

### Zukünftige Verbesserungen
   - Integration eines erweiterten Authentifizierungssystems, um den Zugang zu sichern.
   - Optimierung des Webinterfaces für mobile Geräte.
   - Erweiterung um eine MQTT-Schnittstelle für eine bessere Integration in bestehende IoT-Umgebungen.
   - Implementierung der seriellen Schnittstellenfunktionalität zur Kommunikation mit externen Geräten.

### Lizenz
Dieses Projekt steht unter einer proprietären Lizenz. Es ist nicht gestattet, dieses Projekt zu verwenden, zu verändern oder zu verbreiten. Alle Rechte vorbehalten.

### Kontakt
Bei Fragen oder Anregungen zu diesem Projekt steht das Issue-Tracking auf GitHub zur Verfügung, oder kontaktiere mich direkt über meine GitHub-Seite.
  
