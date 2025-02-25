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
| `light`     | `set`        | `red:on`        | Schaltet die rote LED ein.                           |
| `light`     | `set`        | `red:off`       | Schaltet die rote LED aus.                           |
| `light`     | `set`        | `green:on`      | Schaltet die grüne LED ein.                          |
| `light`     | `set`        | `green:off`     | Schaltet die grüne LED aus.                          |
| `light`     | `set`        | `yellow:on`     | Schaltet die gelbe LED ein.                          |
| `light`     | `set`        | `yellow:off`    | Schaltet die gelbe LED aus.                          |
| `light`     | `blink`      | `red`           | Lässt die rote LED blinken.                          |
| `light`     | `blink`      | `green`         | Lässt die grüne LED blinken.                         |
| `light`     | `blink`      | `yellow`        | Lässt die gelbe LED blinken.                         |
| `light`     | `stopBlink`  | `red`           | Stoppt das Blinken der roten LED.                    |
| `light`     | `stopBlink`  | `green`         | Stoppt das Blinken der grünen LED.                   |
| `light`     | `stopBlink`  | `yellow`        | Stoppt das Blinken der gelben LED.                   |
