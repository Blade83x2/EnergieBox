#!/bin/bash

# Pfade
CONFIG_12V="/Energiebox/12V/config.ini"
CONFIG_230V="/Energiebox/230V/config.ini"
CTRL_12V="/Energiebox/12V/12V"
CTRL_230V="/Energiebox/230V/230V"

# Funktion zur Auswertung einer config.ini
read_and_control() {
    local configFile="$1"
    local ctrlProg="$2"
    local currentRelais=""
    declare -A autoStart
    declare -A autoStop

    while IFS= read -r line || [[ -n $line ]]; do
        if [[ $line =~ ^\[Relais[[:space:]]*([0-9]+)\] ]]; then
            currentRelais="${BASH_REMATCH[1]}"
        elif [[ $line =~ ^autoStart[[:space:]]*=[[:space:]]*([0-9]{2}:[0-9]{2}) ]]; then
            autoStart["$currentRelais"]="${BASH_REMATCH[1]}"
        elif [[ $line =~ ^autoStop[[:space:]]*=[[:space:]]*([0-9]{2}:[0-9]{2}) ]]; then
            autoStop["$currentRelais"]="${BASH_REMATCH[1]}"
        fi
    done < "$configFile"

    # Aktuelle Uhrzeit (HH:MM)
    now=$(date +"%H:%M")

    # Auswertung
    for relais in "${!autoStart[@]}"; do
        #echo "Relais $relais: autoStart=${autoStart[$relais]} autoStop=${autoStop[$relais]}"
        if [[ "$now" == "${autoStart[$relais]}" ]]; then
            #echo "=> Schalte Relais $relais AN ($ctrlProg $relais 1)"
            $ctrlProg "$relais" 1 0
        fi
        if [[ "$now" == "${autoStop[$relais]}" ]]; then
            #echo "=> Schalte Relais $relais AUS ($ctrlProg $relais 0)"
            $ctrlProg "$relais" 0 0
        fi
    done
}

# Aufruf für beide Konfigurationen
read_and_control "$CONFIG_12V" "$CTRL_12V"
read_and_control "$CONFIG_230V" "$CTRL_230V"
