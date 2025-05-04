# #!/usr/bin/env bash

# # Name der Ausgabedatei (relativ zum Skript-Aufrufort)
# OUT="gesamt.txt"

# # 1) Alte Ausgabedatei entfernen (falls vorhanden)
# rm -f "$OUT"

# # 2) Alle Dateien finden, except gesamt.txt, und anhängen
# find . -type f \
#   ! -path "./$OUT" \
#   -print0 | while IFS= read -r -d '' file; do
#     # Trenner mit printf (portabel auf macOS)
#     printf '\n===== Datei: %s =====\n\n' "$file" >> "$OUT"
#     # Inhalt anhängen
#     cat "$file" >> "$OUT"
# done

# echo "Fertig: Inhalt aller Dateien wurde in $OUT geschrieben."

#!/usr/bin/env bash
set -euo pipefail

# Standard-Ausgabedatei
OUT="gesamt.txt"

# Hilfe-Funktion
usage() {
  cat <<EOF
Usage: $0 [-o outputfile] [Pfad1 Pfad2 ...]

Optionen:
  -o FILE    Name der Ausgabedatei (Default: $OUT)
  -h         Diese Hilfe anzeigen

Beispiel:
  $0 -o alle.txt src/ docs/readme.md
EOF
  exit 1
}

# 1) Optionen parsen
while getopts ":o:h" opt; do
  case "$opt" in
    o) OUT="$OPTARG" ;;
    h) usage ;;
    *) usage ;;
  esac
done
shift $((OPTIND -1))

# 2) Wenn keine Pfade angegeben wurden, standardmäßig aktuelles Verzeichnis
if [ "$#" -eq 0 ]; then
  paths=(.)
else
  paths=("$@")
fi

# 3) Alte Ausgabedatei entfernen (falls vorhanden)
rm -f "$OUT"

# 4) Basis-Name der Ausgabedatei (zum Ausschließen in find)
OUT_BASENAME=$(basename "$OUT")

# 5) Über alle übergebenen Pfade iterieren
for path in "${paths[@]}"; do
  if [ -f "$path" ]; then
    # Einzelne Datei
    printf '\n===== Datei: %s =====\n\n' "$path" >> "$OUT"
    cat "$path" >> "$OUT"

  elif [ -d "$path" ]; then
    # Ganzes Verzeichnis (rekursiv)
    find "$path" -type f \
      ! -name "$OUT_BASENAME" \
      -print0 | while IFS= read -r -d '' file; do
        printf '\n===== Datei: %s =====\n\n' "$file" >> "$OUT"
        cat "$file" >> "$OUT"
      done

  else
    echo "Warnung: Pfad nicht gefunden oder kein regulärer Dateityp: $path" >&2
  fi
done

echo "Fertig: Inhalt von [${paths[*]}] wurde in \"$OUT\" zusammengeführt."
