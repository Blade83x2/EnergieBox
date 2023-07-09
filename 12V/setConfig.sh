#!/bin/bash
relais=$(echo $1);
name=$(echo $2); 
activateOnStart=$(echo $3);
pMax=$(echo $4);










if [ "$relais" -eq "5" ]; then
    sudo sed -i "/^\[Relais 5]$/,/^\[/ s/^name = .*/name = $name/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 5]$/,/^\[/ s/^activateOnStart = .*/activateOnStart = $activateOnStart/" /Energiebox/12V/config.ini
    sudo sed -i "/^\[Relais 5]$/,/^\[/ s/^pMax = .*/pMax = $pMax/" /Energiebox/12V/config.ini
fi
