#!/bin/bash
#
# Dieses Script wird nach 12V -set x oder 230V -set x aufgerufen.
# Die vorher eingegebenen Daten (RelaisNr, Name, Synonyme, command & speak) 
# werden dann in die energieboxCommands.yaml geschrieben für die Spracherkennung!
# 
# Nutzung:
# bash /Energiebox/Jarvis/setYaml.sh "<name>" "<tonlaut>" "<command>" "<speak>"
#
# Beispiel:
# bash /Energiebox/Jarvis/setYaml.sh "12V_relais_16_an" "hho, heizung" "/Energiebox/12V/12V 16 1 0" "Heizung wurde eingeschaltet!"
# bash /Energiebox/Jarvis/setYaml.sh "12V_relais_16_aus" "hho, heizung" "/Energiebox/12V/12V 16 0 0" "Heizung wurde ausgeschaltet!"

NAME="$1"
TONLAUT="$2"
COMMAND="$3"
SPEAK="$4"

# YAML Datei auf existens prüfen
YAMLFILE="/Energiebox/Jarvis/energieboxCommands.yaml"
if [[ ! -f "$YAMLFILE" ]]; then
    echo "Fehler: Datei '$YAMLFILE' nicht gefunden!"
    exit 1
fi

# TONLAUT in Kleinbuchstaben umwandeln
TONLAUT=$(printf "%s" "$TONLAUT" | tr '[:upper:]' '[:lower:]')

TMPFILE=$(mktemp)
awk -v name="$NAME" \
    -v tonlaut="$TONLAUT" \
    -v command="$COMMAND" \
    -v speak="$SPEAK" '
BEGIN {
    in_block=0
} {
    # Start des gewünschten Blocks erkennen
    if ($0 ~ "- name: "name"$") {
        in_block=1
        print
        next
    }
    # Neuer Block beginnt -> raus
    if (in_block && $0 ~ /^  - name:/) {
        in_block=0
    }
    # tonlaut ersetzen (nur erste tonlaut Zeile)
    if (in_block && $0 ~ /- tonlaut:/ && !done_tonlaut) {
        indent = substr($0, 1, match($0, /-/)-1)
        print indent "- tonlaut: [" tonlaut "]"
        done_tonlaut=1
        next
    }
    # command ersetzen
    if (in_block && $0 ~ /- command:/) {
        indent = substr($0, 1, match($0, /-/)-1)
        print indent "- command: \"" command "\""
        next
    }
    # speak ersetzen
    if (in_block && $0 ~ /- speak:/) {
        indent = substr($0, 1, match($0, /-/)-1)
        print indent "- speak: \"" speak "\""
        next
    }
    print
}' "$YAMLFILE" > "$TMPFILE"
mv "$TMPFILE" "$YAMLFILE"
