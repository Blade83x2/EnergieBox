#!/bin/bash
newAbwasser=$(echo $1);
echo "; EnergieBox\n; Konfigurationsdatei für h2o\n\n[h2o]\naktuellesGesameltesAbwasser = $newAbwasser" > /Energiebox/h2o/config.ini






