# gui Grafische Benutzeroberfläche 

Dieses Programm stellt die grafische Benutzeroberfläche die bei dem booten der Energiebox geladen wird, zur Verfügung.
Die Anwendung ist für Touchdisplays (800x480) optimiert und läuft als Vollbild-GUI auf dem Raspberry Pi.
---
<br>
Das Programm beinhaltet 3 Tabs: <br> <br>
Auf dem ersten Tab werden live PV- und Batteriedaten angezeigt. 
Diese Daten werden jede 3 Minuten aktualisiert.

<p align="left"> 
    <img src="img/tab_energiebox.png" style="width: 70%;" alt="Tab Energiebox" >
</p>
<br><br>

Auf dem zweiten und dritten Tab wird jeweils eine Geräteliste für die jeweiligen Programme 12V und 230V angezeigt.


<br><br>
12V
<p align="left"> 
    <img src="img/tab_12V.png" style="width: 70%;" alt="Tab 12V" >
</p>
<br><br>
230V
<p align="left"> 
    <img src="img/tab_230V.png" style="width: 70%;" alt="Tab 230V" >
</p>
<br><br>







Eine grafische Steuerungsoberfläche für ein energieautarkes Smart Home-System auf Basis von Relaismodulen, Port-Expandern und einer Wasserstoff-/PV-Stromversorgung. 


## 🔧 Funktionen

- **Tab-basiertes Layout:**
  - **Energiebox:** Anzeige von PV- und Batteriedaten aus `trace.txt`
  - **12V:** Steuerung und Leistungsüberwachung von 12V-Geräten mit Leistungslimit
  - **230V:** Steuerung von 230V-Verbrauchern mit Gesamtleistungsgrenze

- **Automatische Aktualisierung:**
  - Relaisstatus wird alle 10 Sekunden aus `config.ini` aktualisiert
  - Aktualisierung der Beschriftung und Farbe der Buttons bei Änderungen
  - Tracer-Daten werden alle 3 Minuten neu geladen

