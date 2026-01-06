#!/bin/bash
relais="$1"
name="$2"
activateOnStart="$3"
pMax="$4"
autoStart="$5"
autoStop="$6"
canStartFromGui="$7"
execOnStart="$8"
execOnStop="$9"

# Pfad zur Konfigurationsdatei
CONFIG="/Energiebox/12V/config.ini"
if ! [[ "$relais" =~ ^([1-9]|1[0-6])$ ]]; then
  echo "Ungültige Relaisnummer: $relais (erlaubt 1-16)"
  exit 1
fi
# Abschnitt korrekt escapen (eckige Klammern müssen escaped werden)
SECTION="^\\[Relais $relais\\]$"

# Werte setzen
sed -i "/$SECTION/,/^\[/ s/^name = .*/name = $name/" "$CONFIG"
sed -i "/$SECTION/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" "$CONFIG"
sed -i "/$SECTION/,/^\[/ s/^pMax = .*/pMax = $pMax/" "$CONFIG"
sed -i "/$SECTION/,/^\[/ s/^autoStart = .*/autoStart = $autoStart/" "$CONFIG"
sed -i "/$SECTION/,/^\[/ s/^autoStop = .*/autoStop = $autoStop/" "$CONFIG"
sed -i "/$SECTION/,/^\[/ s/^canStartFromGui = .*/canStartFromGui = $canStartFromGui/" "$CONFIG"
escapedExecOnStart=$(printf '%s\n' "$execOnStart" | sed 's/[\/&]/\\&/g')
sed -i "/$SECTION/,/^\[/ s/^execOnStart = .*/execOnStart = $escapedExecOnStart/" "$CONFIG"
escapedExecOnStop=$(printf '%s\n' "$execOnStop" | sed 's/[\/&]/\\&/g')
sed -i "/$SECTION/,/^\[/ s/^execOnStop = .*/execOnStop = $escapedExecOnStop/" "$CONFIG"
