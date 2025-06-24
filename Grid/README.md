# grid Festnetz Batterie- Ladeprogramm

Dieses Programm lädt die Batterien bei Bedarf mit Energie aus dem Festnetz auf.
Hierzu wird zuerst das Ladegerät per Relais mit dem Festnetz verbunden und nach 5 Sekunden wird ein 
weiteres 80 Ampere Relais geschaltet welches die Verbindung zwischen Ladegerät und Batterie herstellt!
---
<br>
Zum Ladestart muss dem Programm über den Parameter <code>-w</code> eine Wattstunden Zahl übergeben
werden. Sobald diese Energie geladen worden ist, schaltet es sich selbstständig wieder ab.
<br>
<br>
Die Eingabe des Befehls <code>grid -w 200</code> startet das Programm und lädt 200 Wattstunden in die Batterie!

<br>
<p align="left"> 
    <img src="img/load.png" style="width: 70%;" alt="grid start load" >
</p>
Damit diese Berechnungen zutreffend sind, müssen in der <code>/Energiebox/Grid/config.ini</code> Datei die Variablen 
angepasst werden an die verfügbare Ladeleistung!
<br><br>
<ul>
    <li>supplyMaxCurrent - Eingestellte Ampere am Ladegerät</li>
    <li>supplyMaxVoltage - Angezeigte Spannung am Ladegerät beim Ladestart</li>
    <li>battVoltageStartLoading - Batteriespannung wenn Ladegerät starten soll</li>
</ul>
<br>
Das Programm berechnet aus diesen Daten die Ladezeit in Sekunden!



<br>



<br>
<br>
Die Eingabe des Befehls <code>grid -s 50 & disown</code> beendet das Programm in 50 Sekunden und gibt die Konsole frei!
<br><br>
<p align="left"> 
    <img src="img/stopload.png" style="width: 70%;" alt="grid stop load" >
</p>
<br><br>




Eine kleine Hilfe gibt es auch. Ungültige Parameterübergabe oder das Aufrufen von <code>grid -h</code> lässt die Hilfe erscheinen!
<br><br>
<p align="left"> 
    <img src="img/help.png" style="width: 70%;" alt="grid Hilfe anzeigen" >
</p><br>




Falls Programm Variablen angepasst werden müssen, kann das über <code>nano /Energiebox/Grid/config.ini</code> realisiert werden.
In dieser Datei muss der Hexadezimalwert als Adresse vom Port Expander vergeben werden. Dieser setzt sich zusammen aus der
gesetzten Jumper Konfiguration auf dem Expander selber. Jede Adresse darf nur einmal pro I²C Kanal vergeben sein.
<br><br>
<p align="left"> 
    <img src="img/config.png" style="width: 70%;" alt="grid config.ini anpassen" >
</p><br>
