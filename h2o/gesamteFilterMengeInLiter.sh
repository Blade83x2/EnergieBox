#!/bin/bash
aktuellesGesameltesAbwasser=$(echo $1);
echo "; EnergieBox\n; Konfigurationsdatei für h2o\n\n[h2o]\naktuellesGesameltesAbwasser = $aktuellesGesameltesAbwasser\ngesamteFilterMengeInLiter = 0" > /Energiebox/h2o/config.ini
