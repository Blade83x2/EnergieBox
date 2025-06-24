#!/bin/bash

# Nur mit Root-Rechten ausführbar
if [[ $EUID -ne 0 ]]; then
    echo "❌ Dieses Skript muss mit sudo ausgeführt werden."
    exit 1
fi

# Prüfen, ob mit sudo aufgerufen wurde
if [ -z "$SUDO_USER" ]; then
    echo "❌ Bitte das Skript mit sudo aufrufen, nicht direkt als root."
    exit 1
fi



# 12V-Berechtigungen
sudo chmod 777 /Energiebox/12V/
sudo chmod 755 /Energiebox/12V/12V
sudo chmod 755 /Energiebox/12V/12V.c
sudo chmod 766 /Energiebox/12V/12V.o
sudo chmod 666 /Energiebox/12V/config.ini
sudo chmod 744 /Energiebox/12V/Makefile
sudo chmod 755 /Energiebox/12V/mymcp23017.c
sudo chmod 755 /Energiebox/12V/mymcp23017.h
sudo chmod 755 /Energiebox/12V/mymcp23017.o
sudo chmod 755 /Energiebox/12V/setConfig.sh
sudo chmod 755 /Energiebox/12V/setIni.sh

# 230V-Berechtigungen
sudo chmod 777 /Energiebox/230V/
sudo chmod 755 /Energiebox/230V/230V
sudo chmod 755 /Energiebox/230V/230V.c
sudo chmod 766 /Energiebox/230V/230V.o
sudo chmod 666 /Energiebox/230V/config.ini
sudo chmod 744 /Energiebox/230V/Makefile
sudo chmod 755 /Energiebox/230V/mymcp23017.c
sudo chmod 755 /Energiebox/230V/mymcp23017.h
sudo chmod 755 /Energiebox/230V/mymcp23017.o
sudo chmod 755 /Energiebox/230V/setConfig.sh
sudo chmod 755 /Energiebox/230V/setIni.sh

# H2O-Berechtigungen
sudo chmod 777 /Energiebox/h2o/
sudo chmod 666 /Energiebox/h2o/config.ini
sudo chmod 755 /Energiebox/h2o/h2o
sudo chmod 755 /Energiebox/h2o/setIni.sh
sudo chmod 755 /Energiebox/h2o/h2o.c
sudo chmod 755 /Energiebox/h2o/h2o.o
sudo chmod 744 /Energiebox/h2o/Makefile



sudo chmod 777 /Energiebox/Tracer/trace.txt

sudo chmod 777 /Energiebox/Kolloid/


sudo chmod 777 /Energiebox/gui/


sudo chmod 777 /Energiebox/Grid/
