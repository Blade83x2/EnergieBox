ERSTINSTALLATION DER ENERGIEBOX SOFTWARE
@Copyright by Johannes a.d.F. K r ä m e r

Es wird davon ausgegangen, dass bereits ein funktionierendes Raspberry OS läuft.
Andernfalls kann eine SD Karte mit dem Programm Raspberry Pi Imager erstellt werden!
Download: https://www.raspberrypi.com/software/

Installationsanweisung:

1) Installation benötigter Pakete & Energiebox
2) Raspi Konfiguration
3) Startup & Shutdown Service
4) PATH Variablen setzen
5) DynDNS Einrichtung für Mobile App Zugriff
6) GPIO Zustände beim starten


-------------------------------------
Installation benötigter Pakete      |
-------------------------------------

Damit alles direkt funktioniert, starten wir zuerst mit der nachträglichen Installation verschiedener benötigter Pakete:

sudo apt-get update && sudo apt-get upgrade -y && sudo apt-get install manpages-de figlet python-dev python-pip git-core -y

sudo pip install rpi.gpio && git clone git://git.drogon.net/wiringPi && cd wiringPi && ./build


-------------------------------------
Installation Energiebox             |
-------------------------------------

Die Energiebox können Sie per ZIP Download herunterladen und sollte unbedingt nach /Energiebox kopieren werden
mit den Rechten 755 oder Sie installieren es bequem mit git clone direkt an die richtige Stelle:
 
 sudo git clone https://github.com/Blade83x2/EnergieBox.git /Energiebox && sudo chmod -R 755 /Energiebox


-------------------------------------
Raspi Konfiguration                 |
-------------------------------------

Als nächstes wird der Raspberry konfiguriert damit die Hardware entsprechend zusammen Arbeiten kann.
WLAN sowie Bluetooth werden ausgeschaltet und das System bekommt einen festen Kabelanschluss für die
Internet Verbindung damit Sie die Eltakos über das Internet steuern können. Hierzu wird der Befehl

sudo nano /boot/config.txt

vom Terminal aus aufgerufen und in der Datei folgendes auskommentiert:

dtoverlay=w1-gpio
dtoverlay=disable-wifi
dtoverlay=disable-bt

Mit dem drücken der Tastenkombination Strg + x kann die Änderung gespeichert werden.
Danach wird die Raspberry Konfiguration aufgerufen mit dem Befehl

sudo raspi-config

Zuerst gehen wir in Interfacing Options rein und aktivieren dort den Service SSH und bestätigen das.
Anschliessend gehen wir wieder in Interfacing Options rein und aktivieren noch den Service I2C auf
die Gleiche Weise. Nun gehen wir in das Menu Network Options unter Hostname und vergeben dort den
Namen EnergieBox.

Danach gehen wir in Boot Options rein, um in das Menü Desktop / CLI zu gelangen. Hier wird Console gewählt.

Nun wählen wir das Menü Advanced Options aus um in das Menü Memory Split zu gelangen. Hier tippen wir den
Wert 64 ein und bestätigen dies mit einem Klick auf OK. Da wir nun fertig sind wählen wir als letztes Finish
und starten den Raspberry neu um die Einstellungen zu übernehmen.

Nach dem Neustart öffnen wir wieder ein Terminal und setzen den Befehl

sudo nano /etc/ssh/sshd_config

ab. In dieser Datei stellen wir sicher das folgender Eintrag gesetzt ist:

PrintLastLog no

Speichern können wir wieder mit der Tastenkombination Strg + x. Danach wird der Befehl

sudo nano /etc/motd

aufgerufen und alles was in dieser Datei steht wird ersetzt mit:

 _____                      _      ____
| ____|_ __   ___ _ __ __ _(_) ___| __ )  _____  __
|  _| | '_ \ / _ \ '__/ _` | |/ _ \  _ \ / _ \ \/ /
| |___| | | |  __/ | | (_| | |  __/ |_) | (_) >  <
|_____|_| |_|\___|_|  \__, |_|\___|____/ \___/_/\_\
                      |___/ V 1.0 by Johannes Krämer

Von Hier aus kann die gesamelte Sonnenenergie über Relais verteilt werden.
Mit den Programmen 12V und 230V werden diese Relais angesteuert.

Schaltbeispiel für 12V Relais:

        Timer in Sek (Optional)__ 
                                 |
      Schaltzustand 1 oder 0__   |
                              |  |
       Relais Nr. 1 bis 16__  |  |
                            | |  |
                   $~/  12V 1 1 90

Schaltbeispiel für 230V Relais:

        Timer in Sek (Optional)__
                                 |
      Schaltzustand 1 oder 0__   |
                              |  |
       Relais Nr. 1 bis 16__  |  |
                            | |  |
                   $~/ 230V 1 1 90

                

Diese Änderung kann ebenfalls wieder mit der Tastenkombination 
Strg + x gespeichert werden. Nun tippen wir auf der Konsole den Befehl 

sudo nano ~/.bashrc

ein und fügen ganz unten am Ende der Datei folgendes ein:

12V
230V

Auch diese Aktion wird wieder mit Strg + x gespeichert.


-------------------------------------
Startup & Shutdown Service          |
-------------------------------------  

Zwecks des Automatischen Einschalten von Stromkreisen (in config.ini einstellbar)
geben wir im Terminal den Befehl

sudo nano /etc/rc.local

ein und fügen ganz unten über exit 0 folgende Zeile ein:

/Energiebox/Startup/mcp_startup

Auch diese Aktion wird wieder mit Strg + x gespeichert.
Danach wird der Befehl

sudo nano /etc/rc.shutdown

eingegeben und in diese Datei wird folgendes rein kopiert:

#!/bin/bash
# rc.shutdown
# Für das deaktivieren der Relais vom MCP23017
/Energiebox/Shutdown/mcp_shutdown
exit 0

Diese Aktion wird wieder mit Strg + x gespeichert.
Als nächstes wird der Befehl

sudo nano /etc/systemd/system/rcshutdown.service

in der Konsole abgesetzt und in diese Datei wird folgendes eingefügt:

[Unit]
Description=/etc/rc.shutdown
Before=shutdown.target

[Service]
ExecStart=/bin/true
ExecStop=/etc/rc.shutdown
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target

Wie immer wird diese Aktion mit Strg + x gespeichert.
Um diesen Service einzuschalten, muss er noch aktiviert werden.
Dies geschieht mit folgendem Terminal Befehl:

sudo systemctl enable rcshutdown.service


-------------------------------------
PATH Variablen                      |
-------------------------------------  

Nun bearbeiten wir noch die PATH Variablen. Dies dient dazu,
das alle Programme die im Ordner /Energiebox/* gespeichert sind
von überall zugänglich sind. Dazu öffnen wir wieder ein Terminal
und tippen den folgenden Befehl ein:

sudo nano /etc/bash.bashrc

am Ende fügen wir die 4 Zeilen hinzu und speichern diese danach
wieder ab mit Strg + x:

PATH=$PATH:/Energiebox/12V
PATH=$PATH:/Energiebox/230V
PATH=$PATH:/Energiebox/Kolloid
PATH=$PATH:/Energiebox/h2o

   
-------------------------------------
IP Setup & DynDNS Einrichtungf      |
-------------------------------------  
                   
Es ist ratsam, die IP Adresszuweisung durch DHCP auf dem Raspberry zu deaktivieren
und anstelle dessen eine Statische IP Adresse zu vergeben. Weiterhin ist es für
diejenigen die Ihre Geräte per Smartphone von unterwegs steuern wollen notwendig,
einen DynDNS Dienst zu wählen. Diejenigen die das nicht möchten, brauchen diese
Schritte nicht zu gehen. Ich werde hier jetzt nicht näher darauf eingehen, da
jede Netzwerkinstallation anders ist und es keine Universal Anleitung dazu gibt.
Eins sei jedoch noch gesagt, wer mit z.B. der Raspberry Controller App Zugang haben
möchte, muss den Standart SFTP Port 22 im Gateway frei geben!


-------------------------------------
GPIO Zustände                       |
-------------------------------------  

Da wir in der Energiebox 32 Kanäle und somit 32 Stromkreise schalten, haben wir
uns natürlich für PortExpander entschieden die über I2C verbunden sind.
Falls es jedoch mal notwendig sein sollte, einen einzelnen GPIO Kanal zu schalten
hat hier ein Beispiel für das Setzen der GPIO Direction (in/out) sowie Wert (nur bei out):

sudo nano /etc/rc.local                  
                   
sleep 0.1
echo "20" > /sys/class/gpio/export
sleep 0.1
echo "out" > /sys/class/gpio/gpio20/direction
echo "1" > /sys/class/gpio/gpio20/value
  





