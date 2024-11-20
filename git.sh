#!/bin/bash

if [ `id -u` -ne 0 ]
  then echo "Script kann nur mit root Rechten aufgerufen werden!"
  exit
fi
cd /Energiebox

cd 230V
sudo make
cd ..

cd 12V
sudo make
cd ..

cd h2o
sudo make
cd ..

cd Shutdown
sudo make
cd ..

cd Startup
sudo make
cd ..

cd Grid
sudo make
cd ..

cd gui
sudo gcc gui.cpp -o gui $(pkg-config --cflags gtk+-3.0 --libs)
cd ..

cd Tracer
sudo gcc -Wall -o tracer tracer.c mymcp23017.o -lwiringPi -std=gnu99
cd ..


git add -A 
git commit -m "$1"
git push -u origin master
# https://github.com/settings/tokens für Token auslesen
# sudo git config --global credential.helper store
