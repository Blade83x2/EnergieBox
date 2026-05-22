#!/bin/bash
#
# Zeigt ASCI Grafik in der Konsole an
#
function showLogo() {
    echo -e "   _  __     _ _       _     _"
    echo -e "  | |/ /___ | | | ___ (_) __| |"
    echo -e "  | ' // _ \| | |/ _ \| |/ _  |"
    echo -e "  | . \ (_) | | | (_) | | (_| |"
    echo -e "  |_|\_\___/|_|_|\___/|_|\__,_|"
    echo -e ""
    echo -e "   ____  _        _   _             "
    echo -e "  / ___|| |_ __ _| |_(_) ___  _ __ "
    echo -e "  \___ \| __/ _  | __| |/ _ \|  _ \ "
    echo -e "  |___) | || (_| | |_| | (_) | | | |"
    echo -e "  |____/ \__\__,_|\__|_|\___/|_| |_|\n\n"
}


#
# Löscht alle Usereingaben wenn z.B. ProgressBar noch läuft
# -> Akzeptiert jede Taste
#
function waitKey() {
    # Alte Tastendrücke verwerfen
    while read -r -t 0; do
        read -r -n 10000 discard
    done
    # Auf neue Taste warten
    read -n 1 -s -r
}

#
# Löscht alle Usereingaben wenn z.B. ProgressBar noch läuft
# -> Akzeptiert nur Enter Taste 
#
function waitEnter() 
{
    # Alte Eingaben verwerfen
    while read -r -t 0; do
        read -r -n 10000 discard
    done
    # Nur ENTER akzeptieren
    while true; do
        IFS= read -r key
        [ -z "$key" ] && break
    done
}


#
# Zeigt Fortschrittsanzeige zum Aufheizen in der Konsole an
#
function ProgressBarHeat()
{
    local current=$1
    local max=$2
    local title="$3"
    local width=29
    # Prozent berechnen
    local percent=$(( current * 100 / max ))
    # Gefüllte Zeichen berechnen
    local done=$(( current * width / max ))
    local left=$(( width - done ))
    # Balken erzeugen
    local fill=$(printf "%${done}s")
    local empty=$(printf "%${left}s")
    # Ausgabe
    printf "\r\e[0m |\e[1;33m > %s: [%s%s] %3d%% \e[0m" \
        "$title" \
        "${fill// /#}" \
        "${empty// /-}" \
        "$percent"
}


#
# Zeigt Fortschrittsanzeige für Plasma Prozess in der Konsole an
#
function ProgressBarRound() 
{
    local current=$1
    local max=$2
    local title="$3"
    local width=26
    # Prozent berechnen
    local percent=$(( current * 100 / max ))
    # Gefüllte Zeichen berechnen
    local done=$(( current * width / max ))
    local left=$(( width - done ))
    # Balken erzeugen
    local fill=$(printf "%${done}s")
    local empty=$(printf "%${left}s")
    # Ausgabe
    printf " \r\e[0m |\e[1;33m > %s: [%s%s] %3d%% \e[0m" \
        "$title" \
        "${fill// /#}" \
        "${empty// /-}" \
        "$percent"
}

#
# Ermittelt ob Benutzer Root Rechte hat:
#
function isSuperUser()
{
    if [ "$(id -u)" == "0" ]; then
	true;
    else
	false;
    fi
    # Aufruf:
    #if isSuperUser ; then
    #    exit 1;
    #fi
}

#
# Prüft ob ein Benutzer kein Root ist:
#
function isNotSuperUser()
{
    if [ "$(id -u)" == "0" ]; then
	false;
    else
	true;
    fi
    # Aufruf:
    #if isNotSuperUser ; then
    #    exit 1;
    #fi
}

#
# Prüft ob eine Verbindung vorhanden ist. 
# Sehr schnell, weil sendet dabei nur ein einziges Paket 
# !!! Aufruf ohne vorangestelltes Protokoll !!!
#
function check_host_reachable() 
{
    local host="$1"
    ping -c 1 -W 1 "$host" &> /dev/null
    # Beispiel:
    #if ! check_host_reachable "kolloidalesgoldkaufen.de"; then
    #    echo "Der Host kolloidalesgoldkaufen.de ist nicht erreichbar! Datenbank kann nicht erreicht werden."
    #    exit 0
    #fi
}

# Funktion: Extrahiere rückwärts alle Zahlen von hinten bis zum ersten Nicht-Ziffern-Zeichen
# Ermittelt aus Produkt ID die PPM
function extract_trailing_digits() 
{
  local input="$1"
  local reversed="$(echo "$input" | rev)"
  local digits=""
  for (( i=0; i<${#reversed}; i++ )); do
    char="${reversed:$i:1}"
    if [[ "$char" =~ [0-9] ]]; then
      digits="$char$digits"
    else
      break
    fi
  done
  echo "$digits"
}

# Funktion: Extrahiere alle Buchstaben von vorne bis zur ersten Zahl
#Ermittelt aus Produkt ID den Metall- Typ
function extract_leading_letters() 
{
  local input="$1"
  local result=""
  for (( i=0; i<${#input}; i++ )); do
    char="${input:$i:1}"
    if [[ "$char" =~ [A-Za-z] ]]; then
      result+="$char"
    else
      break
    fi
  done
  echo "$result"
}

# Funktion: Extrahiert erste Zahl nach Buchstaben
#Ermittelt aus Produkt ID die Menge
function extract_middle_number() 
{
  local input="$1"
  local length=${#input}
  local found_digits=""
  local in_digits=false
  for (( i=0; i<length; i++ )); do
    char="${input:$i:1}"
    if [[ "$char" =~ [0-9] ]]; then
      in_digits=true
      found_digits+="$char"
    elif $in_digits; then
      # Wenn wir schon Ziffern sammeln und auf Buchstaben stoßen: abbrechen
      break
    fi
  done
  echo "$found_digits"
}

# Berechnet aus Gesammt Herstellungsmenge (in ml) die benötigte Gkäsergrö0e zur Produktion
function calculate_count() 
{
  local input=$1
  local sizes=(100 200 400 1000 2000)
  declare -A result
  IFS=$'\n' sizes_sorted=($(printf "%s\n" "${sizes[@]}" | sort -nr))
  local remaining=$input
  for ((i = 0; i < ${#sizes_sorted[@]}; i++)); do
    local size=${sizes_sorted[$i]}
    local count=$(( remaining / size ))
    result["$size"]=$count
    remaining=$(( remaining % size ))
    # Letzte Schleife – prüfe Rest
    if (( i == ${#sizes_sorted[@]} - 1 && remaining > 0 )); then
      result["$size"]=$(( result["$size"] + 1 ))
      remaining=0
    fi
  done
  local res=""
  for size in "${sizes_sorted[@]}"; do
    res="$res,$size:${result[$size]}"
  done
  echo "$res"
}
