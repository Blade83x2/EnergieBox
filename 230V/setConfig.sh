#!/bin/bash
relais="$1"
name="$2"
activateOnStart="$3"
pMax="$4"
autoStart="$5"
autoStop="$6"

# Pfad zur Konfigurationsdatei
CONFIG="/Energiebox/230V/config.ini"
SECTION="^\\[Relais $relais\\]$"

# Werte setzen
sed -i "/$SECTION/,/^\[/ s/^name = .*/name = $name/" "$CONFIG"
sed -i "/$SECTION/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" "$CONFIG"
sed -i "/$SECTION/,/^\[/ s/^pMax = .*/pMax = $pMax/" "$CONFIG"
sed -i "/$SECTION/,/^\[/ s/^autoStart = .*/autoStart = $autoStart/" "$CONFIG"
sed -i "/$SECTION/,/^\[/ s/^autoStop = .*/autoStop = $autoStop/" "$CONFIG"
