#!/bin/bash
scriptPath=$(cd $(dirname "$0"); pwd);
source "$scriptPath/System/spinner.sh" || exit 1; 
clear;
echo -e "\n  _____                      _      ____            ";
echo -e " | ____|_ __   ___ _ __ __ _(_) ___| __ )  _____  __";
echo -e " |  _| | '_ \ / _ \ '__/ _\` | |/ _ \  _ \ / _ \ \/ /";
echo -e " | |___| | | |  __/ | | (_| | |  __/ |_) | (_) >  < ";
echo -e " |_____|_| |_|\___|_|  \__, |_|\___|____/ \___/_/\_\\";
echo -e "                       |___/                        \n\n";

echo -e " ***************************************************";
echo -e " ***             BUILD PROJECT FILES             ***";
echo -e " ***************************************************\n";
if [ "$(id -u)" == "0" ]; then
    echo -e "\n";
else
    echo "Script kann nur mit root Rechten aufgerufen werden!";
    exit 1;
fi
sleep 1
echo -e "Starte Compiler...\n"
sleep 1

start_spinner 'box@Energiebox:/Energiebox/230V:$ make'
sleep 1
cd /Energiebox/230V
sudo make  > /Energiebox/makeResult.txt;
result=$?;
if [ "$result" -eq "2" ]; then
    stop_spinner 1
    cat /Energiebox/makeResult.txt;
    exit 1;
fi
stop_spinner $?


start_spinner 'box@Energiebox:/Energiebox/12V:$ make'
sleep 1
cd /Energiebox/12V
sudo make  > /Energiebox/makeResult.txt;
result=$?;
if [ "$result" -eq "2" ]; then
    stop_spinner 1
    cat /Energiebox/makeResult.txt;
    exit 1;
fi
cd ..
stop_spinner $?


start_spinner 'box@Energiebox:/Energiebox/h2o:$ make'
sleep 1
cd /Energiebox/h2o
sudo make  > /Energiebox/makeResult.txt;
result=$?;
if [ "$result" -eq "2" ]; then
    stop_spinner 1
    cat /Energiebox/makeResult.txt;
    exit 1;
fi
cd ..
stop_spinner $?


start_spinner 'box@Energiebox:/Energiebox/Shutdown:$ make'
sleep 1
cd /Energiebox/Shutdown
sudo make  > /Energiebox/makeResult.txt;
result=$?;
if [ "$result" -eq "2" ]; then
    stop_spinner 1
    cat /Energiebox/makeResult.txt;
    exit 1;
fi
cd ..
stop_spinner $?


start_spinner 'box@Energiebox:/Energiebox/Startup:$ make'
sleep 1
cd /Energiebox/Startup
sudo make  > /Energiebox/makeResult.txt;
result=$?;
if [ "$result" -eq "2" ]; then
    stop_spinner 1
    cat /Energiebox/makeResult.txt;
    exit 1;
fi
cd ..
stop_spinner $?


start_spinner 'box@Energiebox:/Energiebox/Kolloid/sounds:$ make'
sleep 1
cd /Energiebox/Kolloid/sounds
sudo make  > /Energiebox/makeResult.txt;
result=$?;
if [ "$result" -eq "2" ]; then
    stop_spinner 1
    cat /Energiebox/makeResult.txt;
    exit 1;
fi
cd ..
cd ..
stop_spinner $?




start_spinner 'box@Energiebox:/Energiebox/Grid:$ make'
sleep 1
cd /Energiebox/Grid
sudo make  > /Energiebox/makeResult.txt;
result=$?;
if [ "$result" -eq "2" ]; then
    stop_spinner 1
    cat /Energiebox/makeResult.txt;
    exit 1;
fi
cd ..
stop_spinner $?


start_spinner 'box@Energiebox:/Energiebox/gui:$ make'
sleep 1;
cd /Energiebox/gui
sudo make  > /Energiebox/makeResult.txt;
result=$?;
if [ "$result" -eq "2" ]; then
    stop_spinner 1
    cat /Energiebox/makeResult.txt;
    exit 1;
fi
cd ..
stop_spinner $?


start_spinner 'box@Energiebox:/Energiebox/Tracer:$ make'
sleep 1;
cd /Energiebox/Tracer
sudo make clean  > /dev/null
# sudo make format  > /dev/null
sudo make  > /Energiebox/makeResult.txt;
result=$?;
if [ "$result" -eq "2" ]; then
    stop_spinner 1
    cat /Energiebox/makeResult.txt;
    exit 1;
fi
cd ..
stop_spinner $?
rm -f /Energiebox/makeResult.txt;


if [[ -z $1 && -z $2 ]]; then
	echo -e "\n\n";

else
	echo -e "\n\nUpdate Git Repository...";
	git add -A
	echo -e "\n";
	git commit -m "$1"
	echo -e "\n";
	git push -u origin master
	echo -e "\n";
fi

# https://github.com/settings/tokens für Token auslesen
# sudo git config --global credential.helper store
# Usage without Git Commit
#  sudo ./build.sh
# with Git Commit
#  sudo ./build.sh "Beschreibung der Änderung angeben!"
