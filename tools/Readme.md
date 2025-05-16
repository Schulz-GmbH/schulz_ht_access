## Anleitung zum Datei-Zusammenführer

Diese Anleitung beschreibt die Nutzung des Python-Skripts **merge.py**, mit dem du den Inhalt mehrerer Dateien und Verzeichnisse in eine einzige Ausgabedatei zusammenführen kannst. Du kannst das Skript sowohl über die Kommandozeile als auch über eine einfache GUI starten.

---

### 1. Voraussetzungen

-   Python 3.6 oder neuer installiert
-   Optional: Schreibrechte im Verzeichnis, in dem die Ausgabedatei erstellt werden soll.

---

### 2. Installation

1. Skript herunterladen oder in dein Projektverzeichnis kopieren, z. B. als `merge.py`.
2. (Optional) Skript ausführbar machen:

    ```bash
    chmod +x merge.py
    ```

---

### 3. CLI-Modus (Terminal)

Du kannst das Skript direkt im Terminal aufrufen und Parameter übergeben:

```bash
python3 merge.py [Optionen] [Pfad1 Pfad2 ...]
```

#### Verfügbare Optionen

-   `-o, --output <Datei>`

    -   Legt den Namen der Ausgabedatei fest.
    -   **Standard**: `gesamt.txt`

-   `-f, --filter-comments`

    -   Entfernt einzeilige (`#`, `//`) und mehrzeilige (`/* ... */`) Kommentare aus dem Inhalt.

-   `[Pfad1 Pfad2 ...]`

    -   Liste der Dateien und/oder Verzeichnisse, deren Inhalt zusammengeführt werden soll.

#### Beispiele

-   Inhalte aus `src/` und `README.md` in `output.txt` zusammenführen:

    ```bash
    python3 merge.py -o output.txt src/ README.md
    ```

-   Gleiches, aber Kommentare herausfiltern:

    ```bash
    python3 merge.py -o output.txt -f src/ README.md
    ```

---

### 4. GUI-Modus

Wenn du das Skript ohne Parameter startest, öffnet sich eine grafische Benutzeroberfläche:

```bash
./merge.py
# oder
python3 merge.py
```

#### Funktionen der GUI

1. **Dateien hinzufügen**

    - Öffnet einen Dialog zum Auswählen einzelner Dateien.

2. **Ordner hinzufügen**

    - Öffnet einen Dialog zum Auswählen ganzer Verzeichnisse.

3. **Entfernen**

    - Entfernt markierte Einträge aus der Liste.

4. **Ausgabedatei**

    - Eingabefeld für den Namen der resultierenden Datei (Standard: `gesamt.txt`).

5. **Kommentare herausfiltern**

    - Checkbox: Einzeilige und mehrzeilige Kommentare werden beim Zusammenführen entfernt.

6. **Starten**

    - Beginnt den Zusammenführungsprozess und zeigt nach Abschluss eine Meldung.

7. **Abbrechen**

    - Schließt die GUI, ohne etwas zu ändern.

---

### 5. Hinweise

-   Binärdateien oder Dateien mit unbekannter Kodierung werden übersprungen und in der Ausgabedatei mit einer entsprechenden Warnung vermerkt.
-   Die Ausgabedatei selbst wird vor dem Schreiben gelöscht, falls sie bereits existiert.

---

### 6. FAQ

-   **Warum fehlt meine Ausgabedatei im Ergebnis?**

    Das Skript schließt die Datei mit demselben Namen wie die Ausgabedatei aus, um Endlosschleifen zu vermeiden.

-   **Kann ich Kommentare mit anderen Mustern filtern?**

    Derzeit werden nur `#`, `//` und `/* ... */` erkannt. Weitere Muster können im Code ergänzt werden.

---

Viel Erfolg beim Zusammenführen deiner Dateien!
