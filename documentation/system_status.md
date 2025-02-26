# 📊 ESP32 System Status Übersicht

Diese Dokumentation beschreibt die verschiedenen **Systemstatus-Codes**, die über die **Status-LEDs (Rot, Gelb, Grün)** angezeigt werden.

## 🔵 Allgemeine LED-Bedeutung

-   🟥 **Rot** → Fehler / Kein Gerät verbunden
-   🟩 **Grün** → System läuft / Erfolgreiche Aktionen
-   🟨 **Gelb** → Logging / Prozesse in Bearbeitung

---

## 🛠 **Systemstatus-Codes**

| Status-Code                  | Bedeutung                                               | LED-Anzeige                              |
| ---------------------------- | ------------------------------------------------------- | ---------------------------------------- |
| `STATUS_INITIALIZING`        | System wird gestartet                                   | 🟩 **Grüne LED blinkt langsam**          |
| `STATUS_READY`               | System bereit                                           | 🟩 **Grüne LED dauerhaft an**            |
| `STATUS_NO_SD_CARD`          | SD-Karte fehlt                                          | 🟥 **Rote LED dauerhaft an**             |
| `STATUS_NO_HTML_DIR`         | `/www/html` fehlt                                       | 🟥 **Rote LED blinkt 3x langsam**        |
| `STATUS_NO_LOGS_DIR`         | `/logs` fehlt                                           | 🟥 **Rote LED blinkt 2x langsam**        |
| `STATUS_LOG_FILE_ERROR`      | Log-Datei existiert, kann aber nicht beschrieben werden | 🟩 **Grüne LED an**, 🟨 **Gelbe LED an** |
| `STATUS_LOG_WRITE`           | Log-Datei wird geschrieben                              | 🟨 **Gelbe LED blinkt schnell**          |
| `STATUS_NO_DEVICE_CONNECTED` | Kein Gerät mit AP verbunden                             | 🟥 **Rote LED dauerhaft an**             |
| `STATUS_DEVICE_CONNECTED`    | Ein Gerät ist mit dem AP verbunden                      | 🟥 **Rote LED aus**                      |

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
