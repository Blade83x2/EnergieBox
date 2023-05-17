#!/bin/bash
# Richtet alle wichtigen Variablen ein. Manueller Aufruf:
# sudo sh setup.sh pumpeRelaisNr gpd warnLimitAbFilterMenge filterZeitFuerNullKommaEinsLiterInSekunden faktorGefiltertZuAbwasser maxLiterAbwasserKanister reinigungszeitInSekunden aktuellesGesameltesAbwasser gesamteFilterMengeInLiter
# sudo sh setup.sh 6 50 5000.f 42 2 10.f 180 0.000000 0

cd /Energiebox/h2o

pumpeRelaisNr=$(echo $1);
gpd=$(echo $2);
warnLimitAbFilterMenge=$(echo $3);
filterZeitFuerNullKommaEinsLiterInSekunden=$(echo $4);
faktorGefiltertZuAbwasser=$(echo $5);
maxLiterAbwasserKanister=$(echo $6);
reinigungszeitInSekunden=$(echo $7);
aktuellesGesameltesAbwasser=$(echo $8);
gesamteFilterMengeInLiter=$(echo $9);


sed -i "s/int pumpeRelaisNr = .*/int pumpeRelaisNr = "$pumpeRelaisNr";/" h2o.c;
sed -i "s/int gpd = .*/int gpd = "$gpd";/" h2o.c;
sed -i "s/float warnLimitAbFilterMenge = .*/float warnLimitAbFilterMenge = "$warnLimitAbFilterMenge";/" h2o.c;
sed -i "s/int filterZeitFuerNullKommaEinsLiterInSekunden = .*/int filterZeitFuerNullKommaEinsLiterInSekunden = "$filterZeitFuerNullKommaEinsLiterInSekunden";/" h2o.c;
sed -i "s/int faktorGefiltertZuAbwasser = .*/int faktorGefiltertZuAbwasser = "$faktorGefiltertZuAbwasser";/" h2o.c;
sed -i "s/float maxLiterAbwasserKanister = .*/float maxLiterAbwasserKanister = "$maxLiterAbwasserKanister";/" h2o.c;
sed -i "s/int reinigungszeitInSekunden = .*/int reinigungszeitInSekunden = "$reinigungszeitInSekunden";/" h2o.c;

sed -i "s/aktuellesGesameltesAbwasser = .*/aktuellesGesameltesAbwasser = "$aktuellesGesameltesAbwasser"/" config.ini;
sed -i "s/gesamteFilterMengeInLiter = .*/gesamteFilterMengeInLiter = "$gesamteFilterMengeInLiter"/" config.ini;

sleep 1;
sudo make
