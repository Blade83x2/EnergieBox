# Grafische Benutzeroberfläche & Status Monitoring

Dieses Programm stellt die grafische Benutzeroberfläche die bei dem booten der Energiebox gestartet wird, zur Verfügung.
Die Anwendung ist für Touchdisplays (800x480) optimiert und läuft als Vollbild-GUI auf dem Raspberry Pi.

<br>
Das Programm beinhaltet 3 Tabs: <br> <br>
Auf dem ersten Tab werden live MPPT Daten sowie Batterie- Ladedaten angezeigt.  <br>
Diese Anzeige wird jede 3 Minuten aktualisiert.  <br>

<br>
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
Die Geräte können direkt von hier aus mit einem Tastendruck aus bzw. eingeschaltet werden wenn:
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
Es kann vom Raspberry aus lokal mit der Tastenkombination <code>ALT + F4</code> oder mit dem Befehk<br>
<code>kill -9 $(pidof gui)</code>geschlossen werden.<br>
<br>

## Fernsteuerung & System Monitoring über SSH:<br>

Falls eine SSH Verbindung zur Energiebox hergestellt ist, kann dieses Programm auch auf einem <br>
entfernten (Linux) Rechner aufgerufen werden. Dies ist nützlich für das Überwachen der<br>
MPPT & Batterie Daten. Die Relais können ebenfalls bedient werden!<br>


Um das Programm von einem entfernten Rechner zu starten, muss eine SSH Verbindung zur Energiebox<br>
hergestellt werden! Achtung: `X11Forwarding` muss auf der Energiebox erlaubt sein und der ssh Befehl<br>
muss mit dem Parameter -X gestartet werden! Beispiel:<p>
<code>ssh -X box@10.0.0.2</code></p>





<code>kill -9 $(pidof gui)</code> beendet werden.<br>
<br>




Zum Starten des Programms <code>gui</code> ausführen!


<!--
## 🔧 Programm Parameter

- **Tab-basiertes Layout:**
  - **Energiebox:** Anzeige von PV- und Batteriedaten aus `trace.txt`
  - **12V:** Steuerung und Leistungsüberwachung von 12V-Geräten mit Leistungslimit
  - **230V:** Steuerung von 230V-Verbrauchern mit Gesamtleistungsgrenze

- **Automatische Aktualisierung:**
  - Relaisstatus wird alle 10 Sekunden aus `config.ini` aktualisiert
  - Aktualisierung der Beschriftung und Farbe der Buttons bei Änderungen
  - Tracer-Daten werden alle 3 Minuten neu geladen

-->
