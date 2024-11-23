#!/bin/bash
source $(pwd)/spinner.sh

#if [ `id -u` -ne 0 ] then 
 # echo "Script kann nur mit root Rechten aufgerufen werden!";
 # return 1;
#fi



start_spinner 'cd /Energiebox'
sleep 1
cd /Energiebox
stop_spinner $?

start_spinner 'copying non-existen files...'
sleep 3
cp 'file1' 'file2' > /dev/null 2>&1
stop_spinner $?


start_spinner 'cd /Energiebox/230V'
sleep 1
cd /Energiebox/230V
sudo make
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/12V'
sleep 1
cd /Energiebox/12V
sudo make
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/h2o'
sleep 1
cd /Energiebox/h2o
sudo make
cd ..
stop_spinner $?


start_spinner 'cd /Energiebox/Shutdown'
sleep 1
cd /Energiebox/Shutdown
sudo make
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/Startup'
sleep 1
cd /Energiebox/Startup
sudo make
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/Grid'
sleep 1
cd /Energiebox/Grid
sudo make
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/gui'
sleep 1
cd /Energiebox/gui
sudo make
cd ..
stop_spinner $?

start_spinner 'cd /Energiebox/Tracer'
sleep 1
cd /Energiebox/Tracer
sudo make
cd ..
stop_spinner $?


start_spinner 'git add -A'
sleep 1
git add -A
stop_spinner $?

start_spinner 'git commit -m "$1"'
sleep 1
git commit -m "$1"
stop_spinner $?

start_spinner 'git push -u origin master'
sleep 1
git push -u origin master
stop_spinner $?




# https://github.com/settings/tokens für Token auslesen
# sudo git config --global credential.helper store
