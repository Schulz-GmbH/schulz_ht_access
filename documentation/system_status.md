# 📊 ESP32 System Status Übersicht

Diese Dokumentation beschreibt die verschiedenen **Systemstatus-Codes**, die über die **Status-LEDs (Rot, Gelb, Grün)** angezeigt werden.

## 🔵 Allgemeine LED-Bedeutung

-   🟥 **Rot** → Fehler / Kein Gerät verbunden
-   🟩 **Grün** → System läuft / Erfolgreiche Aktionen
-   🟨 **Gelb** → Logging / Prozesse in Bearbeitung

---

## 🛠 **Systemstatus-Codes**

| Status-Code                   | Bedeutung                                               | LED-Anzeige                                                                                 |
| ----------------------------- | ------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| `SYSTEM_INITIALIZING`         | System wird gestartet                                   | 🟩 **Grün blinkt 1× langsam (500ms an/aus), dann Pause.**                                   |
| `SYSTEM_READY`                | System ist bereit                                       | 🟩🟨🟥 **Alle LEDs aus.**                                                                   |
| ----------------------------- | ------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| `SD_CARD_NOT_AVAILABLE`       | SD-Karte oder Modul fehlt                               | 🟥 **Rote LED dauerhaft an (Grün/Gelb aus).**                                               |
| ----------------------------- | ------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| `LOG_NO_DIR`                  | /logs auf SD-Karte fehlt                                | 🟥 **Rot blinkt 2× langsam (500ms an/aus).**                                                |
| `LOG_FILE_ERROR`              | Log-Datei vorhanden, kann aber nicht geschrieben werden | 🟨 **Gelb blinkt 2× langsam (500ms an/aus).**                                               |
| `LOG_WRITE`                   | Schreibt gerade in die Log-Datei                        | 🟩 **Grün blinkt 2× schnell (100ms an/aus)**                                                |
| ----------------------------- | ------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| `WEBSERVER_NO_HTML_DIR`       | /www/html auf SD-Karte fehlt                            | 🟥 **Rot blinkt 3× langsam (500 ms an, 500 ms aus).**                                       |
| ----------------------------- | ------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| `WIFI_STA_NOT_AVAILABLE`      | (STA) Wifi nicht verfügbar                              | 🟥 **Rot blinkt 4× langsam (500 ms an, 500 ms aus).**                                       |
| `WIFI_AP_NOT_AVAILABLE`       | AP nicht verfügbar                                      | 🟥 **Rot blinkt 4× langsam (500 ms an, 500 ms aus).**                                       |
| `WIFI_AP_NO_DEVICE`           | Kein Gerät mit dem Access Point verbunden               | 🟨 **Gelb dauerhaft an (kein Blinken).**                                                    |
| `WIFI_AP_DEVICE_AVAILABLE`    | Mindestens ein Gerät mit dem Access Point verbunden     | 🟨 **Gelb aus (keine Blinkaktion).**                                                        |
| ----------------------------- | ------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| `SERIAL_NOT_CONNECTED`        | Externes serielles Gerät getrennt                       | 🟨 **Gelb blinkt 3× langsam (500ms an/aus).**                                               |
| `SERIAL_CONNECTED`            | Externes serielles Gerät verbunden                      | 🟩 **Grün dauerhaft an (Rot/Gelb aus)..**                                                   |
| `SERIAL_SEND`                 | Sendet eine Nachricht an das externe serielle Gerät     | 🟩 **Grün blinkt 3× schnell (100ms an/aus).**                                               |
| ----------------------------- | ------------------------------------------------------- | ------------------------------------------------------------------------------------------- |

---

## 📌 **Detaillierte LED-Anzeige**

### 🟩 **System Initialisierung (STATUS_INITIALIZING)**

-   **LED**: **Grüne LED blinkt langsam (300ms an / 300ms aus)**
-   **Bedeutung**: System wird hochgefahren
-   **Übergang zu**: `STATUS_READY`

### 🟩 **System bereit (STATUS_READY)**

-   **LED**: **Grüne LED leuchtet dauerhaft**
-   **Bedeutung**: System läuft fehlerfrei

### 🟥 **Kein Gerät mit AP verbunden (STATUS_NO_DEVICE_CONNECTED)**

-   **LED**: **Rote LED dauerhaft an**
-   **Bedeutung**: Kein Gerät ist mit dem WLAN-Access-Point verbunden

### 🟥 **Gerät mit AP verbunden (STATUS_DEVICE_CONNECTED)**

-   **LED**: **Rote LED aus**
-   **Bedeutung**: Mindestens ein Gerät ist mit dem WLAN-AP verbunden

### 🟥 **SD-Karte fehlt (STATUS_NO_SD_CARD)**

-   **LED**: **Rote LED dauerhaft an**
-   **Bedeutung**: Die SD-Karte ist nicht eingelegt oder kann nicht erkannt werden

### 🟥 **HTML-Ordner fehlt (STATUS_NO_HTML_DIR)**

-   **LED**: **Rote LED blinkt 3x langsam (300ms an / 300ms aus)**
-   **Bedeutung**: Das Verzeichnis `/www/html` auf der SD-Karte fehlt

### 🟥 **Logs-Ordner fehlt (STATUS_NO_LOGS_DIR)**

-   **LED**: **Rote LED blinkt 2x langsam (500ms an / 500ms aus)**
-   **Bedeutung**: Das Verzeichnis `/logs` auf der SD-Karte fehlt

### 🟩🟨 **Log-Datei kann nicht geschrieben werden (STATUS_LOG_FILE_ERROR)**

-   **LEDs**: **Grüne LED an, Gelbe LED an**
-   **Bedeutung**: Die Log-Datei existiert, kann aber nicht geschrieben werden

### 🟨 **Log wird geschrieben (STATUS_LOG_WRITE)**

-   **LED**: **Gelbe LED blinkt schnell (100ms an / 100ms aus)**
-   **Bedeutung**: Aktuelle Log-Daten werden in die Datei geschrieben

---

## 🚀 **Zustandsänderungen und Übergänge**

| Startzustand                 | Aktion                      | Neuer Zustand                |
| ---------------------------- | --------------------------- | ---------------------------- |
| `STATUS_INITIALIZING`        | Systemstart abgeschlossen   | `STATUS_READY`               |
| `STATUS_READY`               | Kein Gerät mit AP verbunden | `STATUS_NO_DEVICE_CONNECTED` |
| `STATUS_NO_DEVICE_CONNECTED` | Gerät verbindet sich mit AP | `STATUS_DEVICE_CONNECTED`    |
| `STATUS_NO_SD_CARD`          | SD-Karte eingelegt          | `STATUS_READY`               |
| `STATUS_NO_HTML_DIR`         | `/www/html` wird erstellt   | `STATUS_READY`               |
| `STATUS_NO_LOGS_DIR`         | `/logs` wird erstellt       | `STATUS_READY`               |
| `STATUS_LOG_FILE_ERROR`      | Fehler behoben              | `STATUS_READY`               |
| `STATUS_READY`               | Log wird geschrieben        | `STATUS_LOG_WRITE`           |

---

## 📝 **Changelog**

-   **v1.0**: Erste Version der Status-Dokumentation
