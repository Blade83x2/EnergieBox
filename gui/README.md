# Energiebox GUI

**Version:** 1.4  
**Autor:** Johannes Krämer  
**Datum:** 19.06.2025

Eine grafische Steuerungsoberfläche für ein energieautarkes Smart Home-System auf Basis von Relaismodulen, Port-Expandern und einer Wasserstoff-/PV-Stromversorgung. Die Anwendung ist für Touchdisplays (800x480) optimiert und läuft als Vollbild-GUI auf dem Raspberry Pi.

---

## 🔧 Funktionen

- **Tab-basiertes Layout:**
  - **Energiebox:** Anzeige von PV- und Batteriedaten aus `trace.txt`
  - **12V:** Steuerung und Leistungsüberwachung von 12V-Geräten mit Leistungslimit
  - **230V:** Steuerung von 230V-Verbrauchern mit Gesamtleistungsgrenze

- **Automatische Aktualisierung:**
  - Relaisstatus wird alle 10 Sekunden aus `config.ini` aktualisiert
  - Aktualisierung der Beschriftung und Farbe der Buttons bei Änderungen
  - Tracer-Daten werden alle 3 Minuten neu geladen

- **Sicherheitsfunktionen:**
  - Leistungsgrenzenprüfung vor jedem Schaltvorgang
  - Schaltsperre möglich per `canStartFromGui=0` in der INI
  - Inaktivitäts-Timer: Rückkehr zur Info-Seite nach 3 Minuten

- **INI-gestützte Konfiguration:**
  - Beliebig viele Relais je System (bis max. 16 pro Seite)
  - Konfigurierbare Namen, Leistungen und Startzustände

---

## 🖥️ Systemvoraussetzungen

- Raspberry Pi (getestet auf Pi 4/5)
- GTKmm 3 (C++-Binding für GTK)
- Relaismodule mit Bash-gesteuerter Ansteuerung (`/Energiebox/12V/12V` usw.)
- `trace.txt` aus `/Energiebox/Tracer/` (z. B. via Cronjob erzeugt)

---

## 📂 Verzeichnisstruktur

```text
/Energiebox/
├── 12V/
│   ├── 12V              # Binary zur Ansteuerung eines 12V-Relais
│   └── config.ini       # Konfiguration der 12V-Relais
├── 230V/
│   ├── 230V             # Binary zur Ansteuerung eines 230V-Relais
│   └── config.ini       # Konfiguration der 230V-Relais
└── Tracer/
    └── trace.txt        # Messwerte (Spannung, Strom, Leistung, SOC)
