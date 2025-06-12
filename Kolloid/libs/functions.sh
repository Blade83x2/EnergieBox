#!/bin/bash
#
# Zeigt ASCI Grafik in der Konsole an
#
function showLogo() {
    echo -e " _  __     _ _       _     _ ____  _        _   _             "
    echo -e "| |/ /___ | | | ___ (_) __| / ___|| |_ __ _| |_(_) ___  _ __  "
    echo -e "| ' // _ \| | |/ _ \| |/ _  \___ \| __/ _  | __| |/ _ \|  _ \ "
    echo -e "| . \ (_) | | | (_) | | (_| |___) | || (_| | |_| | (_) | | | |"
    echo -e "|_|\_\___/|_|_|\___/|_|\__,_|____/ \__\__,_|\__|_|\___/|_| |_|"
}

#
# Zeigt Fortschrittsanzeige in der Konsole an
#
function ProgressBar() {
    let _progress=(${1}*100/${2}*100)/100;
    let _done=(${_progress}*4)/10;
    let _left=40-$_done;
    _fill=$(printf "%${_done}s");
    _empty=$(printf "%${_left}s")  ;                    
    # 1.2.1.1 Progress : [########################################] 100%
    printf "\r\e[44m > ${3}: [${_fill// /#}${_empty// /-}] ${_progress}%% \e[0m";
    # usage
    #_start=1;
    #_end=500;
    #for number in $(seq ${_start} ${_end})
    #do
    #    sleep 0.1;
    #    ProgressBar ${number} ${_end};
    #done
    #printf '\nFinished!\n';
}

#
# Ermittelt ob Benutzer Root Rechte hat:
#
isSuperUser()
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
isNotSuperUser()
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
check_host_reachable() {
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
extract_trailing_digits() {
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
extract_leading_letters() {
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
extract_middle_number() {
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
calculate_count() {
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








