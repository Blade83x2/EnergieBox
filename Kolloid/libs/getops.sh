#!/bin/bash
# Liesst Parameter beim Programmaufruf aus. Falls nicht gesetzt werden diese in der Konsole abgefragt

function usage() {
    clear;
    showLogo
    cat << EOF
    
    
    # @Name:            $programmName
    # @Description:     Programm für die automatische Herstellung von Kolloidalen Metalen
    # @Version:         $programmVersion
    # @Copyright:       Johannes a.d.F. K r ä m e r
    # @Trafo            Relais Nr. $trafoRelais
    # @Heizplatte       Relais Nr. $heizplatteRelais
        
    Scriptaufruf: $ kolloid [Options]
    
    Parameter           Beschreibung
    -----------------------------------------------------------------------------
    
    -h,     help,       Zeigt diese Hilfe an

    -m,     metal,      Metalauswahl [${kolloidElementsArray[*]}]

    -s,     size,       Dispersions Menge in ml [${kolloidWaterSizeArray[*]}]
    
    -p,     ppm,        Konzentration in PPM [1 ... 100]

    
    -----------------------------------------------------------------------------
    
    Beispiel Aufruf für 100ml Gold mit 8PPM:
    kolloid -m au -s 100 -p 8
    
EOF
    exit 0;
}

while getopts ":m:s:p:h:" startupParams; do
    case "${startupParams}" in
        m)
            metal=${OPTARG}
            metal=$(echo "$metal" | tr 'A-Z' 'a-z')
            # Prüfen ob übergebener Wert für Metalauswahl in ${kolloidElementsArray} vorhanden ist
            if [[ ! " ${kolloidElementsArray[*]} " =~ " ${metal} " ]]; then
                usage
            fi
            ;;
        s)
            size=${OPTARG}
            # Prüfen ob übergebener Wert für Size in ${kolloidWaterSizeArray} vorhanden ist
            if [[ ! " ${kolloidWaterSizeArray[*]} " =~ " ${size} " ]]; then
                usage
            fi
            ;;
        p)
            ppm=${OPTARG}
            # Prüfen ob übergebener Wert Numerisch ist
            if ! [[ $ppm =~ $ppmPattern ]] ; then
                usage
            fi
            #  und ob $ppm <= 0 oder $ppm > 100 ist
            if [ "$ppm" -le "0" ] || [[ "$ppm" -gt "100" ]]; then
                usage
            fi
            ;;
        h)
            usage
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))
showLogo

# Kolloid
if [ -z "${metal}" ]; then  # Wenn -m nicht übergeben worden ist
    while true; do
        echo -e "$Green"
        echo -n " > Kolloid wählen [${kolloidElementsArray[*]}]: "; 
        read read_metal echo;
        # Prüfen ob übergebener Wert für Metalauswahl in Array vorhanden ist
        if [[ ! " ${kolloidElementsArray[*]} " =~ " ${read_metal} " ]]; then
            echo -e "\n$Red > Kolloid nicht vorhanden! Bitte wiederholen: $ColorOff"
        else
            metal=$read_metal
            echo -e "$ColorOff"
            break;
        fi
    done
else
    # Prüfen ob Wert von Parameter -m in ${kolloidElementsArray} vorhanden ist
    if [[ ! " ${kolloidElementsArray[*]} " =~ " ${metal} " ]]; then
        usage        
    fi
fi

# Menge (ml)
if [ -z "${size}" ]; then # Wenn -s nicht übergeben worden ist
    while true; do
        echo -e "$Green";
        echo -n " > Produktionsmenge in ml wählen [${kolloidWaterSizeArray[*]}]: "; 
        read read_size echo;
        # Prüfen ob übergebener Wert für Mengenauswahl in Array vorhanden ist
        if [[ ! " ${kolloidWaterSizeArray[*]} " =~ " ${read_size} " ]]; then
            echo -e "\n$Red > Produktionsmenge nicht vorhanden! Bitte wiederholen: $ColorOff";
        else
            size=$read_size;
            echo -e "$ColorOff";
            break;
        fi
    done
else
    # Prüfen ob Wert von Parameter -s in ${kolloidWaterSizeArray} vorhanden ist
    if [[ ! " ${kolloidWaterSizeArray[*]} " =~ " ${size} " ]]; then
        usage        
    fi
fi

# PPM
if [ -z "${ppm}" ]; then # Wenn -p nicht übergeben worden ist
    while true; do
        echo -e "$Green";
        echo -n " > PPM Konzentration wählen [1 - 100]: "; 
        read read_ppm echo;
        # Prüfen ob übergebener Wert Numerisch ist
        if ! [[ $read_ppm =~ $ppmPattern ]] ; then
            echo -e "\n$Red > PPM Konzentration nicht zwischen [1 - 100]! Bitte wiederholen: $ColorOff";
        else 
            #  und ob $read_ppm <= 0 oder $read_ppm > 100 ist
            if [ "$read_ppm" -le "0" ] || [[ "$read_ppm" -gt "100" ]]; then
                echo -e "\n$Red > PPM Konzentration nicht zwischen [1 - 100]! Bitte wiederholen: $ColorOff";
            else
                ppm=$read_ppm;
                echo -e "$ColorOff";
                break;
            fi
        fi

    done
fi

