/*
 * Controller & Management Software for H2O Osmose Filters  
 * Vendor: Johannes Krämer
 * Version: 1.0
 * Date: 15.05.2023
*/
#include <stdio.h>
#include <stdlib.h> // atoi()
#include <wiringPi.h> // rasperry Pi
#include <wiringPiI2C.h>  // rasperry Pi
#include "mymcp23017.h"
#include <unistd.h> // sleep()
#include <string.h>
#include <stdbool.h> 
#include <ctype.h>


// 12V Relais Nr. für Pumpe & Boosterpumpe
int pumpeRelaisNr = 6;

// Filterleistung der Anlage in GPD
int gpd = 50;

// Warnung ab x Liter Filterleistung (Für Filtertausch Information)
float warnLimitAbFilterMenge = 1500.f;

// Dauer für 0,1 Liter Wasser zu filtern in Sekunden
int filterZeitFuerNullKommaEinsLiterInSekunden = 42;

// Faktor Abwassermenge zu Filtermenge. Beispiel: 
// Bei 1:2 (1 Liter gefiltertes Wasser und 2 Liter Abwasserproduktion) hier eine 2 eintragen
int faktorGefiltertZuAbwasser = 2;

// Maximale Litermenge im Abwasser Behälter
float maxLiterAbwasserKanister = 18.f;

// Spülzeit/Reinigungszeit in Sekunden
int reinigungszeitInSekunden = 150;

///////////////////////////////////
// AB HIER NICHTS MEHR ÄNDERN//////
///////////////////////////////////

// Liter Pro Galone
float literProGalone = 3.7854f;

// Filtermenge (wird von param1 überschrieben)
float filterMenge = 0.f;

// Filterlaufzeit in Sekunden (float)
float filterLaufzeit = 0.f;

// Filterlaufzeit in Sekunden (int)
int filterLaufzeit_int;

// Abwassermenge für diese Filterung (filtermenge * faktorGefiltertZuAbwasser)
float abwasserMenge = 0.f;

// Aktuell gesameltes Abwasser (wird bei param1 = -empty auf Null gesetzt)
float aktuellesGesameltesAbwasser = 0.f;

// Literanzahl des gesamt gefilterten Wassers seit Filterwechsel
float gesamteFilterMengeInLiter = 0.f;

// System Kommandos String
char command[100];

// h20 Struktur für INI Datei
typedef struct { float aktuellesGesameltesAbwasser; float gesamteFilterMengeInLiter; } h2o_setup;
typedef struct { h2o_setup h2o; } configuration;

// Funktionen deklarieren
static int handler(void* config, const char* section, const char* name, const char* value);
void clearSystem();
void setup();
void changeFilter();
void showHelp();
void showLogo();
void abwasserZaehlerReset();

// Programmstart mit oder ohne Parameter
int main(int argc, char* argv[]) { 
    configuration config;
    // Konfiguration von h2o in config laden
    if (ini_parse("/Energiebox/h2o/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/h2o/config.ini'\n");
        return 1;
    }
    // Bildschirm löschen
    printf("\033[2J\033[1;1H");
    // Wenn Pumpe an, dann ausschalten
    system("12V $pumpeRelaisNr 0");
    showLogo();
    if(argc == 1) {
        // Keine Parameter übergeben, Hilfe anzeigen und beenden
        showHelp();
        return 0;
    }
    // prüfen ob argv[1] -clean oder -empty oder -change oder -setup oder -l enthält, wenn nicht, Hilfe anzeigen
    const char * param1 = argv[1];
    if ( (strcmp(param1, "-clean") == 1) || (strcmp(param1, "-setup") == 1) || (strcmp(param1, "-empty") == 1) || (strcmp(param1, "-change") == 1) || (strcmp(param1, "-l") == 1)) {
        showHelp();
        return 0;
    }
    // wenn ein Parameter übergeben worden ist
    if(argc == 2) {
	// Wenn param1 gleich -clean ist, Anlage spülen
        if (strcmp(param1, "-clean") == 0) {
            clearSystem();
            return 0;
        }
        // Wenn param1 gleich change ist, wurde filter gewechselt und Zähler wird auf 0 gesetzt
        else if (strcmp(param1, "-change") == 0) {
            changeFilter();
            return 0;
        }
        // Wenn param1 gleich -empty ist, Literzähler für Abwasserabfang Kanister auf 0 setzen
        else if (strcmp(param1, "-empty") == 0) {
            abwasserZaehlerReset();
            return 0;
        }
        // Wenn param1 gleich -setup ist, Konfigurationsprogramm aufrufen
        else if (strcmp(param1, "-setup") == 0) {
            setup();
            return 0;
        }
        else {
            showHelp();
            return 0;
        }
        return 0;
    }
    if(argc == 3) {
        // Filtern, wenn param1 -l ist und param2 eine float Wert ist
        const char * param1 = argv[1];
        // Wenn param1 gleich -l ist
        if (strcmp(param1, "-l") == 0) {
            // und param2 größer als 0.1 ist
            filterMenge = (float) atof(argv[2]);
            if (filterMenge >= 0.1){
                // Filtermenge anzeigen
                printf("\n\n -> Filtermenge:\t\t\t%5.1f Liter\n", filterMenge);
                // Filterlaufzeit berechnen
                filterLaufzeit = ((filterMenge / 0.1) * filterZeitFuerNullKommaEinsLiterInSekunden);
                // Typecast in Integer
                filterLaufzeit_int = (int)filterLaufzeit; 
                // und anzeigen
                int s = filterLaufzeit_int;
                int stunden = s / 3600;
                s = s % 3600;
                int minuten =  s / 60;
                s = s % 60;
                int sekunden = s;     
                // Abwassermenge berechnen
                // und anzeigen
                abwasserMenge = filterMenge * faktorGefiltertZuAbwasser;
                printf(" -> Berechnete Abwasser Menge:\t\t%5.1f Liter\n", abwasserMenge);
                // Kannistergröße anzeigen
                printf(" -> Größe des Abwasser Tanks:\t\t%5.1f Liter\n", maxLiterAbwasserKanister);
                // Aktuellen Abwasser Tankfüllstand auslesen
                aktuellesGesameltesAbwasser = config.h2o.aktuellesGesameltesAbwasser;
                gesamteFilterMengeInLiter = config.h2o.gesamteFilterMengeInLiter;
                printf(" -> Aktueller Abwasser Tankfüllstand:\t%5.1f Liter\n", aktuellesGesameltesAbwasser);
                // Maximal mögliches Abwasser ausrechnen und anzeigen
                printf(" -> Restliche mögliche Abwasser Menge:\t%5.1f Liter\n", (maxLiterAbwasserKanister - aktuellesGesameltesAbwasser));
                // Gesamt gefilterte Literzahl der Filterkartusche anzeigen
                printf(" -> Bislang gefiltertes Wasser:\t\t%5.0f Liter\n", gesamteFilterMengeInLiter);
                // Empfohlene maximale Nutzungsleistung in Liter
                printf(" -> Max. Empfohlene Nutzungsmenge:\t%5.0f Liter\n", warnLimitAbFilterMenge);
                // Prüfen ob WarnMenge für gesamte gefilterte Menge ereicht ist
                if(gesamteFilterMengeInLiter >= warnLimitAbFilterMenge) {
                    printf("\e[0;31m -> Die maximal empfohlene Nutzungsmenge des Filters\n    von %f Litern ist erreicht / überschritten.\n    Der Filter sollte gewechselt werden!\e[0m", gesamteFilterMengeInLiter);
                }
                // Prüfen ob Abwasser Menge + Kannisterinhalt nicht mehr als maximale füllmenge ist
                if( (abwasserMenge + aktuellesGesameltesAbwasser) <= maxLiterAbwasserKanister) {
                    // Neue Gesammelt Abwassermenge in Konfiguration speichern
                    sprintf(command, "sudo sh /Energiebox/h2o/setIni.sh %f %f", (aktuellesGesameltesAbwasser+abwasserMenge), (gesamteFilterMengeInLiter+filterMenge));
                    system(command);
                    // filter einschalten
                    sprintf(command, "12V %d 1", pumpeRelaisNr);
                    system(command);
                    printf(" -> Benötigte Zeit: %02d:%02d:%02d\n", stunden, minuten, sekunden);      
                    printf(" -> WASSER WIRD GEFILTERT! BITTE WARTEN...\n");
                    // filter nach x ausschalten                    
                    sprintf(command, "12V %d 0 %d", pumpeRelaisNr, filterLaufzeit_int);
                    system(command);
                    printf(" -> FERTIG!\n");
                }
                else {
                    // Abwasser Kannister hat nicht genug freies Volumen für geplante Filterung. Programm beenden
                    printf("\e[0;31m -> Die verfügbare Abwasser Menge ist zuviel.\n    Zuerst Abwasser Tank entleeren mit ./h2o -empty!\n\n\e[0m");
                    return 0;
                }
                printf("\n");
                return 0;

            } else {
                printf("\n\e[0;31m Die minimale Wassermenge zum filtern muss mindestens 0.1 Liter betragen!\n\n\e[0m");
            }
        }
        else {
            // Bei 2 Parametern kan param1 nur -l sein, wenn nicht, Hilfe anzeigen
            showHelp();
            return 0;
        }
    }
}

// Handler für configurationfile
static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("h2o", "aktuellesGesameltesAbwasser")) { pconfig->h2o.aktuellesGesameltesAbwasser = atof(value); }
    if(MATCH("h2o", "gesamteFilterMengeInLiter")) { pconfig->h2o.gesamteFilterMengeInLiter = atof(value); }    
    else { return 0; }
    return 1;
}

// Setzt Zähler von gesamt gefiltertem Wasser in der aktuellen Filterkartusche auf 0
void changeFilter() {
    // Konfiguration laden
    configuration config;
    // Konfiguration von h2o in config laden
    if (ini_parse("/Energiebox/h2o/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/h2o/config.ini'\n");
    }
    aktuellesGesameltesAbwasser = config.h2o.aktuellesGesameltesAbwasser;
    char answerchange;
    printf("\n\n -> Wurde der Sedimentfilter & Aktivkohlefilter sowie die Membrane gewechselt? Y/N: ");
    scanf("%c", &answerchange);
    if (answerchange == 'Y' || answerchange == 'y' || answerchange == 'J' || answerchange == 'j'){
        sprintf(command, "sudo sh /Energiebox/h2o/gesamteFilterMengeInLiter.sh %f", aktuellesGesameltesAbwasser);
        system(command);
    }
}

// Programm Setup aufrufen und Filtereigenschaften auswählen
void setup() {
    configuration config;
    // Konfiguration von h2o in config laden
    if (ini_parse("/Energiebox/h2o/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/h2o/config.ini'\n");
    } 
    char answersetup;
    printf("\n-> Einrichtung starten um neue Filteranlage zu konfigurieren? Y/N: ");
    scanf("%c", &answersetup);
    if (answersetup == 'Y' || answersetup == 'y' || answersetup == 'J' || answersetup == 'j'){
        // Wenn ja, alle Daten zusammen erfragen und externe Datei aufrufen
        int p1;
        printf("-> An welchem Relais vom 12V Block (1 bis 16) sind die Wasserpumpen angeschlossen?: ");
        scanf("%d", &p1);
        int p2;
        printf("-> Wieviel GPD hat die Filteranlage?: ");
        scanf("%d", &p2);
        int p3;
        printf("-> Wieviel (fertige) Liter Wasser sollte maximal mit einem Satz Filter gefiltert werden?: ");
        scanf("%d", &p3);
        int p4;
        printf("-> Wie viele Sekunden braucht die Filteranlage um exakt 0.1 Liter gefiltertes Wasser zu produzieren?: ");
        scanf("%d", &p4);
        int p5;
        printf("-> Wie ist das Verhältnis von gefiltertem Wasser zu Abwasser/Spülwasser? 1 zu: ");
        scanf("%d", &p5);
        int p6;
        printf("-> Wieviel Liter kann der Abwasser/Spülwasser Tank aufnehmen?: ");
        scanf("%d", &p6);  
        int p7;
        printf("-> Wieviele Sekunden sollen bei einer Spüllung gespüllt werden?: ");
        scanf("%d", &p7);  
        sprintf(command, "sudo sh /Energiebox/h2o/setup.sh %d %d %d.f %d %d %d.f %d 0.000000 0", p1, p2, p3, p4, p5, p6, p7);
        system(command);
    }
}

// Anlage Reinigen per Durchspüllung (Parameter -clean)
void clearSystem() {
    configuration config;
    // Konfiguration von h2o in config laden
    if (ini_parse("/Energiebox/h2o/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/h2o/config.ini'\n");
    } 
    // Zuerst Abwasser Tanks prüfen ob leer
    if(config.h2o.aktuellesGesameltesAbwasser == 0){
        // Hinweisen das Ventil gedreht werden muss
        char answerclear;
        printf("\n\n -> Wurde das Ventil auf Spülung gestellt? Y/N: ");
        scanf("%c", &answerclear);
        if (answerclear == 'Y' || answerclear == 'y' || answerclear == 'J' || answerclear == 'j'){
            // Wenn ja, Spülen
            // filter einschalten
            sprintf(command, "12V %d 1", pumpeRelaisNr);
            system(command);
            sprintf(command, "12V %d 0 %d", pumpeRelaisNr, reinigungszeitInSekunden);
            system(command);
            printf(" -> REINIGUNG BEENDET!\n\n");
        }
    }
    else {
        printf("\n\e[0;31m Bitte vor dem Spülen zuerst den Abwasser Tank leeren mit h2o -empty\n\n\e[0m");
    }
}

// Abwasser Kanister Literzähler auf 0 setzen (Parameter -empty)
void abwasserZaehlerReset() {
    // Abfragen ob Eimer gelert worden ist. 
    char answer;
    printf("\n\n -> Wurde der Abwasser Tank entleert? Y/N: ");
    scanf("%c", &answer);
    if (answer == 'Y' || answer == 'y' || answer == 'J' || answer == 'j'){
        // Konfiguration laden
        configuration config;
        // Konfiguration von h2o in config laden
        if (ini_parse("/Energiebox/h2o/config.ini", handler, &config) < 0) {
            printf("Can't load '/Energiebox/h2o/config.ini'\n");
        }
        gesamteFilterMengeInLiter = config.h2o.gesamteFilterMengeInLiter;
        // Wenn ja, ini auf 0 setzen
        sprintf(command, "sudo sh /Energiebox/h2o/setIni.sh %d %f", 0, gesamteFilterMengeInLiter);
        system(command);
        printf(" -> Abwasser Tank auf 0 Liter eingestellt!\n\n");
    }
}

// Zeigt die Hilfe an
void showHelp() {
    printf("\n\e[0;31m Tool für automatische Filtersteuerung. Anwendungsbeispiel:\n\n");
    printf("  ./h2o -l 0.2  [Produziert 0,2 Liter gefiltertes Wasser]\n");
    printf("  ./h2o -clean  [Spült die Anlage durch]\n");
    printf("  ./h2o -change [Filter austauschen und Zähler auf 0 setzen]\n");
    printf("  ./h2o -empty  [Setzt Tank für Abwasser auf 0 Liter]\n");
    printf("  ./h2o -setup  [Filter konfigurieren]\n\n");
}

void showLogo() {
    printf("\n\n   _   _ ____   ___    ____            _           \n");         
    printf("  | | | |___ \\ / _ \\  / ___| _   _ ___| |_ ___ _ __ ___     \n");
    printf("  | |_| | __) | | | | \\___ \\| | | / __| __/ _ \\ '_ ` _ \\    \n");
    printf("  |  _  |/ __/| |_| |  ___) | |_| \\__ \\ ||  __/ | | | | |   \n");
    printf("  |_| |_|_____|\\___/  |____/ \\__, |___/\\__\\___|_| |_| |_|   \n");
    printf("                              |___/   by Johannes Krämer    \n"); 
}
