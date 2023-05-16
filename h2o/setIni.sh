#!/bin/bash
newAbwasser=$(echo $1);
gesamteFilterMengeInLiter=$(echo $2);
echo "; EnergieBox\n; Konfigurationsdatei für h2o\n\n[h2o]\naktuellesGesameltesAbwasser = $newAbwasser\ngesamteFilterMengeInLiter = $gesamteFilterMengeInLiter" > /Energiebox/h2o/config.ini






