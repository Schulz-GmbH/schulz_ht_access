# Mögliche Event-Antworten

## WiFi-Handler

| **Event** | **Action** | **Status**   | **Details**                  | **Error**         |
| --------- | ---------- | ------------ | ---------------------------- | ----------------- |
| wifi      | get        | on           | SSID: <SSID>                 |                   |
| wifi      | get        | off          | no ssid or password          | no ssid           |
| wifi      | set        | success      | SSID: <SSID>                 |                   |
| wifi      | set        | error        |                              | invalid format    |
| wifi      | connect    | on           | Connected to: <SSID>         |                   |
| wifi      | connect    | off          |                              | connection failed |
| wifi      | disconnect | off          | WiFi disconnected            |                   |
| wifi      | status     | connected    | Connected to: <SSID>         |                   |
| wifi      | status     | disconnected | not connected to any network |                   |
| wifi      | error      | unknown      |                              | unknown setting   |

---

## Light-Handler

| **Event** | **Action** | **Status** | **Details**                   | **Error**     |
| --------- | ---------- | ---------- | ----------------------------- | ------------- |
| light     | set        | on         | red light on                  |               |
| light     | set        | off        | red light off                 |               |
| light     | set        | on         | green light on                |               |
| light     | set        | off        | green light off               |               |
| light     | set        | on         | yellow light on               |               |
| light     | set        | off        | yellow light off              |               |
| light     | blink      | on         | red light blinking            |               |
| light     | blink      | on         | green light blinking          |               |
| light     | blink      | on         | yellow light blinking         |               |
| light     | stopBlink  | off        | red light stopped blinking    |               |
| light     | stopBlink  | off        | green light stopped blinking  |               |
| light     | stopBlink  | off        | yellow light stopped blinking |               |
| light     | error      | unknown    |                               | unknown value |

---

## Serial-Handler

| **Event** | **Action** | **Status** | **Details**                       | **Error**                   |
| --------- | ---------- | ---------- | --------------------------------- | --------------------------- |
| serial    | set        | success    | Baudrate gesetzt: <Baudrate>      |                             |
| serial    | set        | error      |                                   | invalid baud rate           |
| serial    | disconnect | success    | Serial connection closed          |                             |
| serial    | disconnect | error      |                                   | no active serial connection |
| serial    | send       | success    | Command added to queue: <Command> |                             |
| serial    | send       | error      |                                   | queue full                  |
| serial    | receive    | success    | Received data                     | queue full                  |
| serial    | error      | unknown    |                                   | unknown serial setting      |

---

## System-Handler

| **Event** | **Action** | **Status** | **Details**                     | **Error**              |
| --------- | ---------- | ---------- | ------------------------------- | ---------------------- |
| system    | get        | success    | Firmware-Version: 1.0.0         |                        |
| system    | update     | success    | Update gestartet mit URL: <URL> |                        |
| system    | error      | unknown    |                                 | unknown system setting |

---

# Allgemeine Struktur der Antworten

```json
{
	"event": "<event>", // Haupt-Event, z.B. wifi, light, serial, system
	"action": "<action>", // Aktion, z.B. get, set, connect, blink
	"status": "<status>", // Status, z.B. on, off, success, error
	"details": "<details>", // Zusätzliche Informationen zur Aktion
	"error": "<error>" // Fehlerbeschreibung, falls vorhanden
}
```
