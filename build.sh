#!/bin/bash
#
# https://github.com/settings/tokens für Token auslesen
# sudo git config --global credential.helper store
#
# Usage without Git Commit
#
# sudo ./build.sh
#
# Usage with Git Commit
#
# sudo ./build.sh "Beschreibung der Änderung angeben!"
#
scriptPath="$(cd "$(dirname "$0")" && pwd)"
source "$scriptPath/System/spinner.sh" || exit 1
clear
cat << "EOF"

  _____                      _      ____
 | ____|_ __   ___ _ __ __ _(_) ___| __ )  _____  __
 |  _| | '_ \ / _ \ '__/ _` | |/ _ \  _ \ / _ \ \/ /
 | |___| | | |  __/ | | (_| | |  __/ |_) | (_) >  <
 |_____|_| |_|\___|_|  \__, |_|\___|____/ \___/_/\_\
                       |___/

 ***************************************************
 ***             BUILD PROJECT FILES             ***
 ***************************************************

EOF

# Root prüfen
if [[ $(id -u) -ne 0 ]]; then
    echo "Script kann nur mit root Rechten aufgerufen werden!"
    exit 1
fi

echo -e "\nStarte Compiler...\n"
BASE="/Energiebox"
RESULT="$BASE/makeResult.txt"

build() {
    local dir="$1"
    start_spinner "box@Energiebox:$dir:$ make"
    cd "$BASE/$dir" || exit 1
    sudo make clean  > /dev/null
    sudo make format > /dev/null
    sudo make        > "$RESULT"
    local result=$?
    if [[ $result -eq 2 ]]; then
        stop_spinner 1
        cat "$RESULT"
        exit 1
    fi
    stop_spinner $result
}

projects=(
    "230V"
    "12V"
    "h3o2"
    "Shutdown"
    "Startup"
    "Kolloid/sounds"
    "Grid"
    "gui"
    "Status"
    "Tracer"
)

for p in "${projects[@]}"; do
    build "$p"
done

rm -f "$RESULT"

# Optional Git Update
if [[ -n $1 || -n $2 ]]; then
    echo -e "\n\nUpdate Git Repository..."
    git add -A
    git commit -m "$1"
    git push -u origin master
fi

echo -e "\n\nFertig.\n"

