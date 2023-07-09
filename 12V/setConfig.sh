#!/bin/bash
relais=$(echo $1);
name=$(echo $2); 
activateOnStart=$(echo $3);
pMax=$(echo $4);


if [ "$relais" -eq "1" ]; then
    sudo sed -i "/^\[Relais 1]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 1]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 1]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "2" ]; then
    sudo sed -i "/^\[Relais 2]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 2]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 2]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "3" ]; then
    sudo sed -i "/^\[Relais 3]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 3]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 3]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "4" ]; then
    sudo sed -i "/^\[Relais 4]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 4]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 4]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "5" ]; then
    sudo sed -i "/^\[Relais 5]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 5]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 5]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "6" ]; then
    sudo sed -i "/^\[Relais 6]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 6]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 6]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "7" ]; then
    sudo sed -i "/^\[Relais 7]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 7]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 7]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "8" ]; then
    sudo sed -i "/^\[Relais 8]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 8]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 8]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "9" ]; then
    sudo sed -i "/^\[Relais 9]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 9]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 9]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "10" ]; then
    sudo sed -i "/^\[Relais 10]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 10]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 10]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "11" ]; then
    sudo sed -i "/^\[Relais 11]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 11]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 11]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "12" ]; then
    sudo sed -i "/^\[Relais 12]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 12]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 12]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "13" ]; then
    sudo sed -i "/^\[Relais 13]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 13]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 13]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "14" ]; then
    sudo sed -i "/^\[Relais 14]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 14]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 14]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "15" ]; then
    sudo sed -i "/^\[Relais 15]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 15]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 15]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
if [ "$relais" -eq "16" ]; then
    sudo sed -i "/^\[Relais 16]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 16]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 16]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
sleep 5;
