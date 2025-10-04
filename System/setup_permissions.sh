#!/bin/bash
# folgende Befehle erstellen das tree_output.txt File sowie setzen gewisse Ordner auf bestimmte Berechtigungen und s Flags
# Das allerdings nicht bei dem github User erstellen da dort die Berechtigungen von github nicht gesetzt werden!!!

    #echo -e "\n  Ordner Berechtigungen werden auf 770 (rwxrwx---) gesetzt!"
    #sudo find $projektDir -type d -exec chmod 770 {} \;
    
    #echo -e "\n  Setgid-Bit für Verzeichnisse (automatisch Gruppenrechte für neue Dateien übernehmen)"
    #sudo find $projektDir -type d -exec chmod g+s {} \;
    
    #echo -e "\n  Datei Berechtigungen werden auf 660 (rw-rw----) gesetzt!"
    #sudo find $projektDir -type f -exec chmod 660 {} \;

    # Konfigurationsdateien (z.B. *.ini, *.conf) auf 660 (rw-rw----)
    #echo -e "\n  Setze Rechte für Konfigurationsdateien (*.ini, *.conf) auf 660 (rw-rw----)"
    #sudo find $projektDir -type f \( -name "*.ini" -o -name "*.conf" \) -exec chmod 660 {} \;

    # Alle anderen Dateien auf 640 (rw-r-----) – sicherheitshalber
    #echo -e "\n  Setze Rechte für sonstige Dateien auf 640 (rw-r-----)"
    #sudo find $projektDir -type f ! -executable ! \( -name "*.ini" -o -name "*.conf" \) -exec chmod 640 {} \;

    #echo -e "\n  Verzeichnisbaum von \"$projektDir\" wird erstellt!"
    #tree -pug --noreport -i -f $projektDir > tree_output.txt


clear
currentUser="$(whoami)"
printWorkingDirectory="$(pwd)"
projektDir="/Energiebox"
groupName="energiebox"
inputFile="tree_output.txt"
cd $projektDir/System



echo -e "\n  Installationsdatem werden gesammelt für.."
sleep 1
echo -e "  -> Datei & Ordner Berechtigungen"
sleep 1
echo -e "  -> Benutzer & Gruppeneinstellungen\n"
sleep 1
echo -e "  -> Sammle Informationen"
sleep 0.4
echo -e "  Aktueller Benutzer: ${currentUser}"
sleep 0.4
echo -e "  Aktuelles Verzeichnis: ${printWorkingDirectory}\n"
sleep 1
if [[ "$currentUser" != "root" ]]; then
    echo -e "  Gruppen werden geprüft"
    sleep 1
    groupInfo=`grep "${groupName}" /etc/group`
    if [ -n "${groupInfo}" ]; then
        echo -e "  -> Gruppe \"${groupName}\" existiert - ok!\n"
        sleep 1
        echo -e "  Benutzer werden geprüft"
        hasUserRoot=`echo "${groupInfo}" | grep "root"`
        sleep 1
        if [ -n "${hasUserRoot}" ]; then
            echo -e "  -> Benutzer \"root\" befindet sich in der \"${groupName}\" Gruppe - ok!"
            hasCurrentUser=`echo "${groupInfo}" | grep "${currentUser}"`
            sleep 1
            if [ -n "${hasCurrentUser}" ]; then
                echo -e "  -> Benutzer \"${currentUser}\" befindet sich in der \"${groupName}\" Gruppe - ok!\n"
                sleep 1
                echo -e "  -> Struktur wird aus der Datei \"$projektDir/$inputFile\" rekonstruiert!"
                sleep 2
                RED='\033[0;31m'
                NC='\033[0m'  # No Color
                while IFS= read -r line; do
                    # Nur Zeilen mit eckigen Klammern und einem Pfad verarbeiten
                    if [[ "$line" =~ ^\[([^\]]+)\]\ +(.+)$ ]]; then
                        meta="${BASH_REMATCH[1]}"
                        filepath="${BASH_REMATCH[2]}"
                        # Metadaten zerlegen: Rechte, Besitzer, Gruppe
                        perms=$(echo "$meta" | awk '{print $1}')
                        owner=$(echo "$meta" | awk '{print $2}')
                        group=$(echo "$meta" | awk '{print $3}')
                        # Nur fortfahren, wenn die Datei existiert
                        if [[ -e "$filepath" ]]; then
                            # Besitzer und Gruppe setzen
                            echo "  sudo chown \"$owner:$group\" \"$filepath\""
                            sudo chown "$owner:$group" "$filepath"
                            # Rechte extrahieren (rwxrwxrwx)
                            rights="${perms:1:9}"
                            # Basisrechte ermitteln
                            mode=$(echo "$rights" | awk '
                                {
                                    perm = "";
                                    for (i = 1; i <= 3; i++) {
                                        n = 0;
                                        r = substr($0, (i - 1) * 3 + 1, 1);
                                        w = substr($0, (i - 1) * 3 + 2, 1);
                                        x = substr($0, (i - 1) * 3 + 3, 1);
                                        if (r ~ /r/) n += 4;
                                        if (w ~ /w/) n += 2;
                                        if (x ~ /x|s|t/) n += 1;
                                        perm = perm "" n;
                                    }
                                    print perm;
                                }')
                            # Spezialbits: Setuid (4), Setgid (2), Sticky (1)
                            special=0
                            [[ "${rights:2:1}" =~ [sS] ]] && special=$((special + 4))
                            [[ "${rights:5:1}" =~ [sS] ]] && special=$((special + 2))
                            [[ "${rights:8:1}" =~ [tT] ]] && special=$((special + 1))
                            final_mode="${special}${mode}"
                            # Berechtigungen setzen
                            echo "  sudo chmod \"$final_mode\" \"$filepath\""
                            sudo chmod "$final_mode" "$filepath"
                            echo -e "\n"
                        else
                            echo -e "  ${RED}WARNUNG: '$filepath' existiert nicht, übersprungen.${NC}"
                            echo -e "\n"
                        fi
                    fi
                done < "$inputFile"
            else
                echo -e "  -> Benutzer \"${currentUser}\" befindet sich nicht in der \"${groupName}\" Gruppe - abbruch!"
                echo -e "  -> FIXIT: sudo usermod -aG ${groupName} ${currentUser}"
            fi
        else
            echo -e "  -> Benutzer \"root\" befindet sich nicht in der \"${groupName}\" Gruppe - abbruch!"
            echo -e "  -> FIXIT: sudo usermod -aG ${groupName} root"
        fi
    else
        echo -e "  -> Gruppe \"${groupName}\" existiert nicht - abbruch!"
        echo -e "  -> FIXIT: sudo groupadd ${groupName}"
    fi
else
    echo -e "  -> Dieses Script darf nicht mit root Rechten aufgerufen werden - abbruch!"
    echo -e "  -> Aufruf: bash $0"
fi
echo -e "\n"
