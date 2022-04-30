@Copyright by Johannes a.d.F. K r ä m e r
---

Willkommen bei der Einrichtung der EnergieBox!

Die EnergieBox ist eine 24 Volt Photovoltaik Anlage mit Batteriespeicher.

Die gesamelte Energie kann dann über Relais an angeschlossene Geräte verteilt werden.



Es wird davon ausgegangen, dass bereits ein funktionierendes Raspberry OS auf dem Raspberry läuft. 

Andernfalls kann eine Raspberry OS SD Karte mit dem Programm Raspberry Pi Imager erstellt werden!

Download: https://www.raspberrypi.com/software/

Diese Textdatei sollte einmal nach einer Neuinstallation des Betriebssystems abgearbeitet werden damit die EnergieBox wieder wie gewohnt auf dem Raspberry 3B+ funktioniert!
Die beiden MCP´s für die Relais müssen am I2C Port auf die Adresse 0x22 (für 12 Volt) und 0x27 (für 230 Volt) gejumpt sein! Die 4 Relais werden dann an den zwei MCP´s angeschlossen!

Ein Buzzer sollte am GPIO Pin 17 eingesteckt sein!

------------------------
Benötigte Bauteile:
------------------------


2x MCP23017

2x Relais 8fach bis 30Volt DC

2x Relais 8fach bis 230Volt, 10A AC

1x Buzzer

---

1) Installation benötigter Pakete
2) Raspi Konfiguration
3) Startup & Shutdown Service
4) PATH Variablen setzen
5) SSH KEY auf Clients einrichten 



-------------------------------------
Installation benötigter Pakete
-------------------------------------

sudo apt-get update && sudo apt-get upgrade -y && sudo apt-get install manpages-de figlet python-dev python-pip -y && sudo pip install rpi.gpio && sudo apt-get install git-core -y && git clone git://git.drogon.net/wiringPi && cd wiringPi && ./build

---

Der Ordner EnergieBox muss ins Root Verzeichnis kopiert werden und folgende Berechtigungen müssen gesetzt werden
 
sudo chmod -R 755 /Energiebox

-------------------------------------
Raspi Konfiguration
-------------------------------------

sudo nano /boot/config.txt

folgendes auskommentieren oder hinzufügen

dtoverlay=w1-gpio

dtoverlay=disable-wifi

dtoverlay=disable-bt

---

sudo raspi-config

(unter Interfaceing Options)

-> SSH aktivieren

-> I2C aktivieren

(unter Network Options -> Hostname)

-> EnergieBox

(unter Boot Options -> Desktop / CLI)

-> Console

(unter Advanced Options -> Memory Split)

-> 64

---

sudo nano /etc/ssh/sshd_config

PrintLastLog no

---

sudo nano /etc/motd


Willkommen auf der EnergieBox. 
Von Hier aus kann die generierte Sonnen- Energie über Relais verteilt werden.
Mit denn Programmen 12V und 230V werden diese Relais angesteuert.

Schaltbeispiel für 12V Relais:

        Timer in Min (Optional)__ 
                                 |
      Schaltzustand 1 oder 0__   |
                              |  |
       Relais Nr. 1 bis 16__  |  |
                            | |  |
                   $~/  12V 1 1 90



Schaltbeispiel für 230V Relais:

        Timer in Min (Optional)__
                                 |
      Schaltzustand 1 oder 0__   |
                              |  |
       Relais Nr. 1 bis 16__  |  |
                            | |  |
                   $~/ 230V 1 1 90

                
---                        

sudo nano ~/.bashrc

und folgendes am Ende einfügen

12V

230V

-------------------------------------
Startup & Shutdown Service
-------------------------------------  
                   
sudo nano /etc/rc.local

vor exit 0

/Energiebox/Startup/mcp_startup

---
                   
sudo nano /etc/rc.shutdown

und folgendes einfügen

#!/bin/bash

/Energiebox/Shutdown/mcp_shutdown

exit 0

---

sudo nano /etc/systemd/system/rcshutdown.service

und folgendes einfügen

[Unit]

Description=/etc/rc.shutdown

Before=shutdown.target

[Service]

ExecStart=/bin/true

ExecStop=/etc/rc.shutdown

RemainAfterExit=yes

[Install]

WantedBy=multi-user.target

---

sudo systemctl enable rcshutdown.service
                  
-------------------------------------
PATH Variablen
-------------------------------------  

sudo nano /etc/bash.bashrc

am Ende hinzufügen

PATH=$PATH:/Energiebox/12V

PATH=$PATH:/Energiebox/230V

PATH=$PATH:/Energiebox/Kolloid


-------------------------------------
SSH KEY auf Clients einrichten
-------------------------------------  
                   
Wer nicht immer das Passwort des PI´s eingeben möchte, der kann das Zugriffspasswort als Key speichern.

Hierzu auf dem Client folgendes eingeben:

ssh-keygen -t rsa -P ""

ssh-copy-id pi@EnergieBox

ssh pi@EnergieBox


