#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>



// Relais Nr. für Pumpe & Boosterpumpe
int pumpeRelaisNr = 6;  

// Filterleistung der Anlage in GPD
int gpd = 50;

// Dauer für 0,1 Liter Wasser zu filtern
int filterZeitFuerNullKommaEinsLiterInSekunden = 42;

// Faktor Abwassermenge zu Filtermenge. Beispiel: 
// Bei 1:2 (1 Liter gefiltertes Wasser und 2 Liter Abwasserproduktion) hier eine 2 eintragen
int faktorGefiltertZuAbwasser = 2;

// Maximale Abwassermenge in Liter bei Reinigung
float maxLiterAbwasserBeiReinigung = 5.f;

// Maximale Litermenge im Abwasser Behälter
float maxLiterAbwasserKanister = 10.f;


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

// Aktuell gesameltes Abwasser (wird bei param1 = -r auf Null gesetzt)
float aktuellesGesameltesAbwasser = 0.f;   //TODO aus ini lesen !!!!!!!!!!!!!


char command[100];


// h20 Struktur für INI Datei
typedef struct { float aktuellesGesameltesAbwasser; } h2o_setup;
typedef struct { h2o_setup h2o; } configuration;

// Handler für configurationfile
static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("h2o", "aktuellesGesameltesAbwasser")) { pconfig->h2o.aktuellesGesameltesAbwasser = atoi(value); }
    else { return 0; }
    return 1;
}

// Funktionen deklarieren
void clearSystem();
void showHelp();
void showLogo();
void abwasserZaehlerReset();

// Programmstart
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
    // prüfen ob argv[1] -c oder -r oder -l enthält, wenn nicht, Hilfe anzeigen
    const char * param1 = argv[1];
    if ( (strcmp(param1, "-c") == 1) || (strcmp(param1, "-r") == 1) || (strcmp(param1, "-l") == 1)) {
        showHelp();
        return 0;
    }
    // wenn ein Parameter übergeben worden ist
    if(argc == 2) {
	// Wenn param1 gleich -c ist, Anlage spülen
        if (strcmp(param1, "-c") == 0) {
            clearSystem();
            return 0;
        }
        // Wenn param1 gleich -r ist, Literzähler für Abwasserabfang Kanister auf 0 setzen
        else if (strcmp(param1, "-r") == 0) {
            abwasserZaehlerReset();
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
        
        
        // TODO In argv[2] Komma mit Punkt ersetzen falls vorhanden

        
        // Wenn param1 gleich -l ist
        if (strcmp(param1, "-l") == 0) {
            // und param2 größer als 0.1 ist
            filterMenge = (float) atof(argv[2]);
            if (filterMenge >= 0.1){
                // Filtermenge anzeigen
                printf("\n\n -> Filtermenge:\t\t\t%f Liter\n", filterMenge);
                // Filterlaufzeit berechnen
                filterLaufzeit = ((filterMenge / 0.1) * filterZeitFuerNullKommaEinsLiterInSekunden);
                // Typecast in Integer
                filterLaufzeit_int = (int)filterLaufzeit; 
                // und anzeigen
                printf(" -> Filterlaufzeit:\t\t\t%d Sekunden\n", filterLaufzeit_int);
                // Abwassermenge berechnen
                // und anzeigen
                abwasserMenge = filterMenge * faktorGefiltertZuAbwasser;
                printf(" -> Berechnete Abwasser Menge:\t\t%f Liter\n", abwasserMenge);
                // Kannistergröße anzeigen
                printf(" -> Größe des Abwasserbehälters:\t%f Liter\n", maxLiterAbwasserKanister);
                // Aktuellen Kannisterfüllstand auslesen
                aktuellesGesameltesAbwasser = config.h2o.aktuellesGesameltesAbwasser;
                printf(" -> Aktueller Kannisterfüllstand:\t%f Liter\n", aktuellesGesameltesAbwasser);
                // Maximal mögliches Abwasser ausrechnen und anzeigen
                printf(" -> Maximal mögliche Abwasser Menge:\t%f Liter\n", (maxLiterAbwasserKanister - aktuellesGesameltesAbwasser));
                // Prüfen ob Abwasser Menge kleiner oder gleich verfügbaren Platz im Abwasser Kannister ist
                if(abwasserMenge <= (maxLiterAbwasserKanister - aktuellesGesameltesAbwasser)) {
                    // Neue Gesammelt Abwassermenge in Konfiguration speichern
                    sprintf(command, "sudo sh /Energiebox/h2o/setIni.sh %f", (aktuellesGesameltesAbwasser+abwasserMenge));
                    system(command);
                    // filter einschalten
                    sprintf(command, "12V %d 1", pumpeRelaisNr);
                    system(command);
                    printf("\n FILTERUNG LÄUFT!\n");
                    // filter nach x ausschalten                    
                    sprintf(command, "12V %d 0 %d", pumpeRelaisNr, filterLaufzeit_int);
                    system(command);
                    printf("\n FILTERUNG FERTIG!\n");
                }
                else {
                    // Abwasser Kannister hat nicht genug freies Volumen für geplante Filterung. Programm beenden
                    printf("\n\e[0;31m Die verfügbare Abwasser Menge ist zuviel. Zuerst Abwasser Kannister entleeren!\n\n");
                    return 0;
                }
                printf("\n");
                return 0;

            } else {
                printf("\n\e[0;31m Die minimale Wassermenge zum filtern muss mindestens 0.1 Liter betragen!\n\n");
            }
        }
        else {
            // Bei 2 Parametern kan param1 nur -l sein, wenn nicht, Hilfe anzeigen
            showHelp();
            return 0;
        }
    }
}

void clearSystem() {
    printf("\nREINIGUNG\n\n");
    // Hinweisen das Ventil gedreht werden muss

    // TODO zuerst Eimer leer machen, dann bestätigen, dann reinigen


}

// Abwasser Kanister Literzähler auf 0 setzen
void abwasserZaehlerReset() {
    printf("\nAbwasser Reset\n\n");
    // Abfragen ob Eimer gelert worden ist. 
    
    
    
    
    // Wenn ja, ini auf 0 setzen
    sprintf(command, "sudo sh /Energiebox/h2o/setIni.sh %d", 0);
    system(command);
}

// Zeigt die Hilfe an
void showHelp() {
    printf("\n\e[0;31m Falsche Parameter! Beispiel:\n\n");
    printf(" h2o -l 0.2 [Produziert 0,2 Liter gefiltertes Wasser]\n");
    printf(" h2o -c [Spült die Anlage durch (Zuerst Ventil öffnen)]\n");
    printf(" h2o -r [Setzt Kanister für Abwasser auf Null]\n\n\e[0m");
}

void showLogo() {
    printf("\n\n\tHHHHHHHHH     HHHHHHHHH 222222222222222         OOOOOOOOO     \n");
    printf("\tH:::::::H     H:::::::H2:::::::::::::::22     OO:::::::::OO   \n"); 
    printf("\tH:::::::H     H:::::::H2::::::222222:::::2  OO:::::::::::::OO  \n");
    printf("\tHH::::::H     H::::::HH2222222     2:::::2 O:::::::OOO:::::::O \n");
    printf("\t  H:::::H     H:::::H              2:::::2 O::::::O   O::::::O \n");
    printf("\t  H:::::H     H:::::H              2:::::2 O:::::O     O:::::O \n");
    printf("\t  H::::::HHHHH::::::H           2222::::2  O:::::O     O:::::O \n");
    printf("\t  H:::::::::::::::::H      22222::::::22   O:::::O     O:::::O \n");
    printf("\t  H:::::::::::::::::H    22::::::::222     O:::::O     O:::::O \n");
    printf("\t  H::::::HHHHH::::::H   2:::::22222        O:::::O     O:::::O \n");
    printf("\t  H:::::H     H:::::H  2:::::2             O:::::O     O:::::O \n");
    printf("\t  H:::::H     H:::::H  2:::::2             O::::::O   O::::::O \n");
    printf("\tHH::::::H     H::::::HH2:::::2       222222O:::::::OOO:::::::O \n");
    printf("\tH:::::::H     H:::::::H2::::::2222222:::::2 OO:::::::::::::OO  \n");
    printf("\tH:::::::H     H:::::::H2::::::::::::::::::2   OO:::::::::OO    \n");
    printf("\tHHHHHHHHH     HHHHHHHHH22222222222222222222     OOOOOOOOO      \n\n");
    printf("\t ______ _  _____ _________    _______  _____ _____ ______      \n");
    printf("\t/    / / \\/__ __/  __/  __\\  / ___\\  \\// ___/__ __/  __/ \\__/| \n");
    printf("\t|  __| | |  / \\ |  \\ |  \\/|  |     \\  /|    \\ / \\ |  \\ | |\\/|| \n");
    printf("\t| |  | | |_/| | |  /_|    /  \\___ |/ / \\___ | | | |  /_| |  || \n");
    printf("\t\\_/  \\_\\____\\_/ \\____\\_/\\_\\  \\____/_/  \\____/ \\_/ \\____\\_/  \\| \n\n");
}
