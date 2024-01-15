#!/bin/bash
relais=$(echo $1);
state=$(echo $2);

if [ "$relais" -eq "1" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 1]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 1]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "2" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 2]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 2]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "3" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 3]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 3]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "4" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 4]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 4]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "5" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 5]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 5]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "6" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 6]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 6]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "7" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 7]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 7]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "8" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 8]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 8]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "9" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 9]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 9]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "10" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 10]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 10]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "11" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 11]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 11]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "12" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 12]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 12]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "13" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 13]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 13]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "14" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 14]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 14]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "15" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 15]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 15]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
if [ "$relais" -eq "16" ]; then
 if [ "$state" -eq "1" ]; then
   sed -i '/^\[Relais 16]$/,/^\[/ s/^eltakoState = 0/eltakoState = 1/' /Energiebox/230V/config.ini
 else
   sed -i '/^\[Relais 16]$/,/^\[/ s/^eltakoState = 1/eltakoState = 0/' /Energiebox/230V/config.ini
 fi
fi
