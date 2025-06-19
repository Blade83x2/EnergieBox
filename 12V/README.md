
# 12V Steuerprogramm

Dieses Programm schaltet maximal 16 Eltakos für 12Volt Stromkreise.
Die Stromstärke beträgt derzeit 60 Ampere bei 12 Volt (720 Watt).
Beim Einschalten eines Gerätes wird geprüft, ob die Notwendige 
angefragte Last verfügbar ist um Überlasten abzufangen.
Ein Timer zum schalten sowie eine Benutzerfreundliche Konfiguration
der Endverbraucher ist ebenfalls implementiert!
<br>
<br>
Listenansicht mit dem Befehl: `12V`<br><br>
<p align="left"> 
    <img src="img/main.png" style="width: 70%;" alt="12V" >
</p>
<br><br>



Falls ein Relais Status (0 oder 1) von einem Relais von einem
anderem Programm ausgelesen werden muss, kann dieser Wert z.B. 
über den | oder >> Operator übergeben werden:
<br><br>
<p align="left"> 
    <img src="img/statusRelais.png" style="width: 70%;" alt="Relais Status von Relais Nr. 4" >
</p><br>




Der Status des Relais ist nun in der Datei status.txt gespeichert!
Falls ein Relais geschaltet werden soll, so kann das mit dem Befehl  
<code>12V 4 1</code> bewerkstelligt werden. Die 4 steht für die Relais Nummer und die
1 für den gewünschten Zustand (Möglich: 0/1). Nach dem absetzen des Befehls wird wieder die Liste angezeigt!
<br><br>
<p align="left"> 
    <img src="img/set.png" style="width: 70%;" alt="Relais Status von Relais Nr. 4 einschalten" >
</p><br>



Möchten man dagegen das Relais 4 einschalten ohne das eine Ausgabe angezeigt wird, dann kann der Befehl
<code>12V 4 1 0</code> ausgeführt werden!
<br><br>
<p align="left"> 
    <img src="img/set_no_stdout.png" style="width: 70%;" alt="Relais Status von Relais Nr. 4 einschalten" >
</p><br>



Um ein Relais einmalig per Befehl von einer Zeitschaltuhr steuern zu lassen, kann der Befehl <code>12V 4 0 300 & disown</code> verwendet 
werden. In diesem Beispiel würde das Relais Nr. 4 ausgeschaltet werden nach 300 Sekunden. Die Parameter <code>& disown</code>
können verwendet werden damit die Konsole wieder sofort verfügbar ist.
<br><br>
<p align="left"> 
    <img src="img/set_time.png" style="width: 70%;" alt="Relais Status von Relais Nr. 4 nach 300 Sekunden ausschalten" >
</p><br>


Wenn ein Relais Eintrag verändert werden soll, z.B. Name, Leistungsaufnahme oder Automatisches Einschalten beim Starten
der Anlage oder eine tägliche Zeitschaltung, dann kann der Befehl <code>12V -set 6</code> verwendet werden. 
Hier würde jetzt das Relais Nr. 6 bearbeitet werden! Wenn der Name leer gelassen wird, werden alle Werte gelöscht.
Es sollte nicht vergessen werden das an der 12V Schalttaffel eine entsprechende Sicherung eingesetzt werden muss!
<br><br>
<p align="left"> 
    <img src="img/set_menu.png" style="width: 70%;" alt="12V Gerät konfigurieren" >
</p><br>





Eine kleine Hilfe gibt es auch. Ungültige Parameterübergabe oder das Aufrufen von <code>12V -h</code> lässt die Hilfe erscheinen!
<br><br>
<p align="left"> 
    <img src="img/help.png" style="width: 70%;" alt="12V Hilfe anzeigen" >
</p><br>







Falls Programm Variablen angepasst werden müssen, kann das über <code>nano /Energiebox/12V/config.ini</code> realisiert werden.
In dieser Datei muss der Hexadezimalwert als Adresse vom Port Expander vergeben werden. Dieser setzt sich zusammen aus der
gesetzten Jumper Konfiguration auf dem Expander selber. Jede Adresse darf nur einmal pro I²C Kanal vergeben sein.
Für maxPConverter muss die Maximale Ausgangsleistung der Spannungswandler in Watt angegeben werden.
Mit der Variable maxPMicroController wird der Leistungsverbrauch der Elektronik selber in Watt angegeben da diese ebenfalls
von den 12V Spannungswandlern versorgt wird!
<br><br>
<p align="left"> 
    <img src="img/config.png" style="width: 70%;" alt="12V config.ini anpassen" >
</p><br>
