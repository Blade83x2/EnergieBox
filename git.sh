#!/bin/bash

if [ `id -u` -ne 0 ]
  then echo "Script kann nur mit root Rechten aufgerufen werden!"
  exit
fi

cd 230V
make
cd ..
cd 12V
make
cd ..
cd h2o
make
cd ..
cd Shutdown
make
cd ..
cd Startup
make
cd ..
cd gui
sudo gcc gui.cpp -o gui $(pkg-config --cflags gtk+-3.0 --libs)
cd ..
cd Tracer
gcc -o tracer tracer.c
cd ..


git add -A 
git commit -m "$1"
git push -u origin master
# https://github.com/settings/tokens für Token auslesen
# sudo git config --global credential.helper store
