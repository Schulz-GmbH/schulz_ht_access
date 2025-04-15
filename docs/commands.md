| **Command** | **Setting**  | **Value**       | **Beschreibung**                                     |
| ----------- | ------------ | --------------- | ---------------------------------------------------- |
| `wifi`      | `get`        |                 | Gibt die gespeicherte SSID und das Passwort zurück.  |
| `wifi`      | `set`        | `ssid:password` | Speichert die SSID und das Passwort.                 |
| `wifi`      | `connect`    |                 | Verbindet sich mit dem gespeicherten WiFi-Netzwerk.  |
| `wifi`      | `disconnect` |                 | Trennt die aktuelle WiFi-Verbindung.                 |
| `wifi`      | `status`     |                 | Fragt den aktuellen Status des Wifis ab              |
| `serial`    | `set`        | `baudRate`      | Setzt die Baudrate und Verbindet `SerialDevice`.     |
| `serial`    | `disconnect` |                 | Trennt die aktuelle Verbindung für `SerialDevice`.   |
| `serial`    | `send`       | `message`       | Sendet eine Nachricht über `SerialDevice`.           |
| `system`    | `get`        | `version`       | Gibt die aktuelle Firmware-Version zurück.           |
| `system`    | `update`     | `url`           | Startet ein Firmware-Update von der angegebenen URL. |
| `log`       | `list`       |                 | Gibt die aktuellen Logs zurück.                      |
| `log`       | `debug`      | `set:on`        | Aktiviert das erweiterte Logging                     |
| `log`       | `debug`      | `set:off`       | Deaktiviert das erweiterte Logging                   |
| `log`       | `debug`      | `status`        | Gibt den Såtatus des erweitereten loggings zurück    |
