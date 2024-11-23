#!/bin/bash
scriptPath=$(cd $(dirname "$0"); pwd);
source "$scriptPath/spinner.sh" || exit 1; 

if [ "$(id -u)" == "0" ]; then
    echo "Script kann nur mit root Rechten aufgerufen werden!";
    return 1;
else
    echo -e "\n"
fi


echo -e "\n"

start_spinner 'cd /Energiebox'
sleep 1
cd /Energiebox
echo -e "\n"
stop_spinner $?



start_spinner 'cd /Energiebox/230V && sudo make'
sleep 1
cd /Energiebox/230V
sudo make  > /dev/null 2>&1
cd ..
stop_spinner $?
sleep 1
start_spinner 'cd /Energiebox/12V && sudo make'

cd /Energiebox/12V
sudo make  > /dev/null 2>&1
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/h2o && sudo make'
sleep 1
cd /Energiebox/h2o
sudo make > /dev/null 2>&1
cd ..
stop_spinner $?


start_spinner 'cd /Energiebox/Shutdown'
sleep 1
cd /Energiebox/Shutdown
sudo make > /dev/null 2>&1
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/Startup'
sleep 1
cd /Energiebox/Startup
sudo make > /dev/null 2>&1
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/Grid'
sleep 1
cd /Energiebox/Grid
sudo make > /dev/null 2>&1
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/gui'
sleep 1
cd /Energiebox/gui
sudo make > /dev/null 2>&1
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/Tracer'
sleep 1
cd /Energiebox/Tracer
sudo make > /dev/null 2>&1
cd ..
stop_spinner $?


start_spinner 'git add -A'
sleep 1
git add -A > /dev/null 2>&1
stop_spinner $?

start_spinner 'git commit -m "$1"'
sleep 1
git commit -m "$1" > /dev/null 2>&1
stop_spinner $?

start_spinner 'git push -u origin master'
sleep 1
git push -u origin master > /dev/null 2>&1
stop_spinner $?




# https://github.com/settings/tokens für Token auslesen
# sudo git config --global credential.helper store
