
# grid Batterie- Netzladeprogramm

Dieses Programm lädt die Batterien bei Bedarf mit Energie aus dem Festnetz auf.
Zum Ladestart muss dem Programm über den Parameter `-w` eine Wattstunden Zahl übergeben
werden. Sobald diese Energie geladen worden ist, schaltet es sich selbstständig wieder ab.
<br>
<br>
Die Eingabe des Befehls `grid -w 200` startet das Programm und lädt 200 Wattstunden in die Batterie!<br><br>
<p align="left"> 
    <img src="img/load.png" style="width: 70%;" alt="grid start load" >
</p>
<br><br>



<br>
<br>
Die Eingabe des Befehls <code>grid -s 50</code> beendet das Programm in 50 Sekunden!<br><br>
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
