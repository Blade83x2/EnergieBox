# Grafische Benutzeroberfläche & Status Monitoring

Dieses Programm stellt die grafische Benutzeroberfläche die bei dem booten der Energiebox gestartet wird,<br>
zur Verfügung. Die Anwendung ist für Touchdisplays (800x480) optimiert und läuft als Vollbild-GUI auf dem Raspberry Pi.

<br>
Das Programm beinhaltet 3 Tabs: <br> <br>
Auf dem ersten Tab werden live MPPT Daten sowie Batterie- Ladedaten angezeigt.  <br>
Diese Anzeige wird jede 3 Minuten aktualisiert.  <br>
<br>
<br>
Tab Energiebox
<p align="left"> 
    <img src="img/tab_energiebox.png" style="width: 70%;" alt="Tab Energiebox" >
</p>
<br><br>
Auf dem zweiten und dritten Tab wird jeweils eine Geräteliste für die jeweiligen Programme<br>
12V und 230V angezeigt.<br><br>
Ein roter Eintrag bedeutet das dieses Gerät derzeit ausgeschaltet<br>
ist und grün zeigt das dieses Gerät gerade mit Spannung versorgt wird und eingeschaltet ist. <br> 
<br>
<br>
Tab 12V
<p align="left"> 
    <img src="img/tab_12V.png" style="width: 70%;" alt="Tab 12V" >
</p>
<br>
Die Geräte können direkt von hier aus mit einem Tastendruck aus bzw. eingeschaltet werden wenn:<br>
<br>
<ul>
  <li>genügend Leistung zum Einschalten verfügbar ist</li>
  <li>die Relais Konfiguration einen Start aus der GUI erlaubt</li>
</ul>
<br>
Wenn z.B. in der Konfiguration von 230V eingetragen wurde, dass der 230 Volt Inverter<br>
eine Dauerleistung von 5000 Watt leistet und derzeit Geräte in der Liste aktiviert sind<br>
die mit einem Verbrauch von 4000 Watt eingetragen sind, dann kann in diesem Moment kein weiteres Gerät<br>
eingeschaltet werden welches mehr als 1000 Watt benötigt. In diesem Fall wird vom Programm<br>
eine Meldung ausgegeben die besagt das nicht genügend Leistung vorhanden ist.<br>
Um dieses Gerät trotzdem einzuschalten, müssen vorher andere Geräte ausgeschaltet werden!<br>
<br>
<br>
Tab 230V
<p align="left"> 
    <img src="img/tab_230V.png" style="width: 70%;" alt="Tab 230V" >
</p>
<br><br>
Das Programm wird beim booten der Energiebox automatisch geladen und auf dem Display angezeigt.<br>
Es kann vom Raspberry aus lokal mit der Tastenkombination <code>ALT + F4</code> oder mit dem Befehl<br>
<code>kill -9 $(pidof gui)</code>geschlossen werden.<br>
<br>

## Fernsteuerung & System Monitoring über SSH:<br>

Falls eine SSH Verbindung zur Energiebox hergestellt ist, kann dieses Programm auch auf einem <br>
entfernten (Linux) Rechner aufgerufen werden. Dies ist nützlich für das Überwachen der<br>
MPPT & Batterie Daten. Die Relais können ebenfalls bedient werden!<br>


Um das Programm von einem entfernten Rechner zu starten, muss eine SSH Verbindung zur Energiebox<br>
hergestellt werden!<br><br>
Achtung: `X11Forwarding yes` muss auf der Energiebox aktiviert sein und der Client muss den ssh Befehl<br>
mit dem Parameter -X starten! Beispiel:<p>
<code>ssh -X box@10.0.0.2</code> oder <br>
<code>ssh -X -p2222 box@home.cplusplus-development.de</code></p>
Nachdem diese Verbindung aufgebaut worden ist, muss zuerst sichergestellt werden. dass das Program <code>gui</code><br>
nicht bereits lokal auf der Energiebox läuft. Um es vom entferntem Client zu beenden, kann der Befehl:<br>
<br><code>kill -9 $(pidof gui) > /dev/null 2> /dev/null</code><br>
<br>
genutzt werden. Um das Programm auf der Energiebox zu starten und über X11Forwarding auf dem Client PC<br>
anzuzeigen, kann der folgende Befehl genutzt werden:<br>
<br>
<code>gui --window</code><br>
<br>


## 🔧 Programm Parameter

Bei falscher Benutzung der Programm Parameter wird eine Hilfe angezeigt.<br>
Hier kann eingesehen werden welche Parameter verfügbar sind und wozu sie genutzt werden!
<br>
<br>
<p align="left"> 
    <img src="img/help.png" style="width: 70%;" alt="gui --help" >
</p>


