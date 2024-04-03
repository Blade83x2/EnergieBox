#!/bin/bash

isSuperUser()
{
    if [ "$(id -u)" == "0" ]; then
        true;
    else
        false;
    fi
}

if isSuperUser ; then
    echo "Bitte als sudo User aufrufen!";
    exit 0;
fi



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
cd gui
sudo gcc gui.cpp -o gui $(pkg-config --cflags gtk+-3.0 --libs)
cd ..
cd Tracer
gcc -o tracer tracer.c
cd ..


sudo git add -A 
sudo git commit -m "$1"
sudo git push -u origin master

