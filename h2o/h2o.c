/*
 * Controller & Management Software for H2O Osmose Filters
 * Vendor: Johannes Krämer
 * Version: 1.0
 * Date: 15.05.2023
 */
#include <stdio.h>
#include <stdlib.h>       // atoi()
#include <wiringPi.h>     // rasperry Pi
#include <wiringPiI2C.h>  // rasperry Pi
#include "iniparse.h"     // INI Parse
#include "mymcp23017.h"   // PortExpander
#include <unistd.h>       // sleep()
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>

// 12V Relais Nr. für Pumpe & Boosterpumpe ODER Ventil
int pumpeRelaisNr = 16;

// Filterleistung der Anlage in GPD                                                     ###############
int gpd = 50;

// Warnung ab x Liter Filterleistung (Für Filtertausch Information)
float warnLimitAbFilterMenge = 1000.f;

// Dauer für 0,1 Liter Wasser zu filtern in Sekunden
int filterZeitFuerNullKommaEinsLiterInSekunden;

// Faktor Abwassermenge zu Filtermenge. Beispiel:
// Bei 1:2 (1 Liter gefiltertes Wasser und 2 Liter Abwasserproduktion) hier eine 2 eintragen
int faktorGefiltertZuAbwasser = 2;

// Maximale Litermenge im Abwasser Behälter
float maxLiterAbwasserKanister = 18.f;

// Spülzeit/Reinigungszeit in Sekunden
int reinigungszeitInSekunden = 150;

// Liter Pro Galone                 ################
float literProGalone = 3.7854f;

// Liter Pro Tag  = literProGalone * gpd
long double literProTag = 0.f;

// Liter pro Sekunde = (literProTag / 86400) / 1000
long double MilliliterProSekunde = 0.f;

// Filtermenge unformatiert (Koma und Punkt annehmen)
char* filterMengeUnformated;

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
char command[101];

// h20 Struktur für INI Datei
typedef struct {
    float aktuellesGesameltesAbwasser;
    float gesamteFilterMengeInLiter;
    int pumpeRelaisNr;
    int gpd;
    float warnLimitAbFilterMenge;
    int filterZeitFuerNullKommaEinsLiterInSekunden;
    int faktorGefiltertZuAbwasser;
    float maxLiterAbwasserKanister;
    int reinigungszeitInSekunden;
} h2o_setup;

typedef struct {
    h2o_setup h2o;
} configuration;

// Funktionen deklarieren
static int handler(void* config, const char* section, const char* name, const char* value);
void clearSystem();
void setup();
void changeFilter();
void showHelp();
void showLogo();
void abwasserZaehlerReset();
void printStatistik();
void replace_char(char* s, char find, char replace);
void print_progress(size_t count, size_t max);
void print_progress2(size_t count, size_t max);
// void SIGhandler(int);

void sigfunc(int sig) {
    printf("\n");
    if (sig == SIGINT) {  // Dies Signal wird an alle Prozesse geschickt wenn die Tasten-Kombination STRG-C gedrückt wurde.
        sprintf(command, "12V %d 0 1", pumpeRelaisNr);
        system(command);
        exit(0);
    }
    if (sig == SIGABRT) {  // Dieses Signal signalisiert das das Programm abnormal beendet wurde (abort()).
        sprintf(command, "12V %d 0 1", pumpeRelaisNr);
        system(command);
        exit(0);
    }
    if (sig == SIGFPE) {  // Diese Signal wird angezeigt z.B. bei einer Division durch 0 oder einem Überlauf einer Zahl.
        sprintf(command, "12V %d 0 1", pumpeRelaisNr);
        system(command);
        exit(0);
    }
    if (sig == SIGILL) {  // Dies wird angezeigt wenn ein illegaler Hardware-Befehl ausgeführt wird.
        sprintf(command, "12V %d 0 1", pumpeRelaisNr);
        system(command);
        exit(0);
    }
    if (sig == SIGSEGV) {  // Wird dies Angezeigt wurde versucht auf eine unerlaubte Speicherstelle zu schreiben oder zu lesen.
        sprintf(command, "12V %d 0 1", pumpeRelaisNr);
        system(command);
        exit(0);
    }
    if (sig == SIGTERM) {  // Voreingestelltes Signal, das das kill-Kommando an einen Prozess schickt, das beendet werden soll.
        sprintf(command, "12V %d 0 1", pumpeRelaisNr);
        system(command);
        exit(0);
    }
    if (sig == SIGBUS) {  // Hardware-Fehler
        sprintf(command, "12V %d 0 1", pumpeRelaisNr);
        system(command);
        exit(0);
    }
    return;
}

// Programmstart mit oder ohne Parameter
int main(int argc, char* argv[]) {
    // #ifdef DBUILD_DATE
    // printf("  Version: %s\n", DBUILD_DATE);
    // printf("  Build Date: %s\n", BUILD_DATE);
    // #endif

    // Signale abfangen
    if (signal(SIGINT, sigfunc) == SIG_ERR || signal(SIGABRT, sigfunc) == SIG_ERR || signal(SIGFPE, sigfunc) == SIG_ERR || signal(SIGILL, sigfunc) == SIG_ERR ||
        signal(SIGSEGV, sigfunc) == SIG_ERR || signal(SIGTERM, sigfunc) == SIG_ERR || signal(SIGBUS, sigfunc) == SIG_ERR) {
        fprintf(stderr, "Signal Fehler!\n");
    }
    configuration config;
    // Konfiguration von h2o in config laden
    if (ini_parse("/Energiebox/h2o/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/h2o/config.ini'\n");
        return 1;
    }
    // Programm Variablen aus config.ini laden
    // Bereits verbrauchte Filtermenge in Liter
    gesamteFilterMengeInLiter = config.h2o.gesamteFilterMengeInLiter;
    // Aktuellen Abwasser Tankfüllstand auslesen
    aktuellesGesameltesAbwasser = config.h2o.aktuellesGesameltesAbwasser;
    // 12V Relais Nr. für Pumpe & Boosterpumpe
    pumpeRelaisNr = config.h2o.pumpeRelaisNr;
    // Filterleistung der Anlage in GPD
    gpd = config.h2o.gpd;
    // Warnung ab x Liter Filterleistung (Für Filtertausch Information)
    warnLimitAbFilterMenge = config.h2o.warnLimitAbFilterMenge;
    // Dauer für 0,1 Liter Wasser zu filtern in Sekunden
    filterZeitFuerNullKommaEinsLiterInSekunden = config.h2o.filterZeitFuerNullKommaEinsLiterInSekunden;
    // Faktor Abwassermenge zu Filtermenge. Beispiel:
    // Bei 1:2 (1 Liter gefiltertes Wasser und 2 Liter Abwasserproduktion) hier eine 2 eintragen
    faktorGefiltertZuAbwasser = config.h2o.faktorGefiltertZuAbwasser;
    // Maximale Litermenge im Abwasser Behälter
    maxLiterAbwasserKanister = config.h2o.maxLiterAbwasserKanister;
    // Spülzeit/Reinigungszeit in Sekunden
    reinigungszeitInSekunden = config.h2o.reinigungszeitInSekunden;
    // Bildschirm löschen
    printf("\033[2J\033[1;1H");
    // Pumpe auf aus sschalten
    sprintf(command, "12V %d 0 1", pumpeRelaisNr);
    system(command);
    showLogo();
    if (argc == 1) {
        // Keine Parameter übergeben, Hilfe anzeigen und beenden
        showHelp();
        return 0;
    }
    const char* param1 = argv[1];
    // wenn ein Parameter übergeben worden ist
    if (argc == 2) {
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
            abwasserZaehlerReset();  ///////////////////////////////////////////////////////////
            return 0;
        }
        // Wenn param1 gleich -setup ist, Konfigurationsprogramm aufrufen
        else if (strcmp(param1, "-setup") == 0) {
            setup();
            return 0;
        } else if (strcmp(param1, "-stats") == 0) {
            printStatistik();
            return 0;
        }
        filterMengeUnformated = argv[1];
        replace_char(filterMengeUnformated, ',', '.');
        filterMenge = (float)atof(filterMengeUnformated);
        if (filterMenge >= 0.1) {
            // Filtermenge anzeigen
            printf("\n\n -> Filtermenge:\t\t\t%5.1f Liter\n", filterMenge);
            // Filterleistung anzeigen
            printf(" -> Filterleistung:\t\t\t   %d GPD\n", gpd);
            // Filterlaufzeit berechnen
            filterLaufzeit = ((filterMenge / 0.1) * filterZeitFuerNullKommaEinsLiterInSekunden);
            // Typecast in Integer
            filterLaufzeit_int = (int)filterLaufzeit;
            // und anzeigen
            int s = filterLaufzeit_int;
            int stunden = s / 3600;
            s = s % 3600;
            int minuten = s / 60;
            s = s % 60;
            int sekunden = s;
            // Abwassermenge berechnen
            abwasserMenge = filterMenge * faktorGefiltertZuAbwasser;
            printf(" -> Berechnete Abwasser Menge:\t\t%5.1f Liter\n", abwasserMenge);
            // Benötigte Sekundenanzahl
            printf(" -> Berechnete Filter Zeit:\t\t%5.0f Sek.\n", filterLaufzeit);
            // Gesamt gefilterte Literzahl der Filterkartusche anzeigen
            printf(" -> Bislang gefiltertes Wasser:\t\t%5.1f Liter\n", gesamteFilterMengeInLiter);
            // Empfohlene maximale Nutzungsleistung in Liter
            printf(" -> Max. Empfohlene Filtermenge:\t%5.0f Liter\n", warnLimitAbFilterMenge);
            // Maximal mögliches Abwasser ausrechnen und anzeigen
            printf(" -> Restliche mögliche Abwasser Menge:\t%5.1f Liter\n", (maxLiterAbwasserKanister - aktuellesGesameltesAbwasser));
            // Aktueller Abwassertank Füllstand
            printf(" -> Aktueller Abwasser Tankfüllstand:\t%5.1f Liter\n", aktuellesGesameltesAbwasser);
            // Kannistergröße anzeigen
            printf(" -> Gesamtgröße des Abwasser Tanks:\t%5.1f Liter\n", maxLiterAbwasserKanister);
            // Prüfen ob WarnMenge für gesamte gefilterte Menge ereicht ist
            if (gesamteFilterMengeInLiter >= warnLimitAbFilterMenge) {
                printf(
                    "\e[0;31m -> Die maximal empfohlene Nutzungsmenge des Filters\n    von %f Litern ist erreicht / überschritten.\n    Der Filter sollte gewechselt werden!\e[0m",
                    gesamteFilterMengeInLiter);
            }
            // Prüfen ob Abwasser Menge + Kannisterinhalt nicht mehr als maximale Füllmenge ist
            if ((abwasserMenge + aktuellesGesameltesAbwasser) <= maxLiterAbwasserKanister) {
                // Neue Gesammelt Abwassermenge in Konfiguration speichern
                sprintf(command, "sh /Energiebox/h2o/setIni.sh %f %f %d %d %f %d %d %f %d", (aktuellesGesameltesAbwasser + abwasserMenge),
                        (gesamteFilterMengeInLiter + filterMenge), pumpeRelaisNr, gpd, warnLimitAbFilterMenge, filterZeitFuerNullKommaEinsLiterInSekunden,
                        faktorGefiltertZuAbwasser, maxLiterAbwasserKanister, reinigungszeitInSekunden);
                system(command);
                // filter nach einer Sekunde einschalten
                sprintf(command, "12V %d 1 1", pumpeRelaisNr);
                system(command);
                printf("\n -> Wasser wird gefiltert. Benötigte Zeit: %02d:%02d:%02d\n", stunden, minuten, sekunden);
                int laufzeitSekunden = 0;
                while (true) {
                    print_progress2(laufzeitSekunden, filterLaufzeit_int);
                    sleep(1);
                    laufzeitSekunden++;
                    if (laufzeitSekunden == filterLaufzeit_int) {
                        print_progress2(filterLaufzeit_int, filterLaufzeit_int);
                        break;
                    }
                }
                // filter nach x ausschalten
                sprintf(command, "12V %d 0 %d", pumpeRelaisNr, 1);
                system(command);
                printf("\n -> FERTIG!\n");
            } else {
                // Abwasser Kannister hat nicht genug freies Volumen für geplante Filterung. Programm beenden
                printf("\e[0;31m -> Die benötigte Abwasser Menge ist zuviel.\n    Zuerst Abwasser Tank entleeren mit h2o -empty!\n\n\e[0m");
                return 0;
            }
            printf("\n");
            return 0;
        } else {
            printf("\n\e[0;31m Die minimale Wassermenge zum filtern muss mindestens 0.1 Liter betragen!\n\n\e[0m");
        }
    }
}

// Handler für configurationfile
static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("h2o", "aktuellesGesameltesAbwasser")) {
        pconfig->h2o.aktuellesGesameltesAbwasser = atof(value);
    }
    if (MATCH("h2o", "gesamteFilterMengeInLiter")) {
        pconfig->h2o.gesamteFilterMengeInLiter = atof(value);
    }
    if (MATCH("h2o", "pumpeRelaisNr")) {
        pconfig->h2o.pumpeRelaisNr = atoi(value);
    }
    if (MATCH("h2o", "gpd")) {
        pconfig->h2o.gpd = atoi(value);
    }
    if (MATCH("h2o", "warnLimitAbFilterMenge")) {
        pconfig->h2o.warnLimitAbFilterMenge = atof(value);
    }
    if (MATCH("h2o", "filterZeitFuerNullKommaEinsLiterInSekunden")) {
        pconfig->h2o.filterZeitFuerNullKommaEinsLiterInSekunden = atoi(value);
    }
    if (MATCH("h2o", "faktorGefiltertZuAbwasser")) {
        pconfig->h2o.faktorGefiltertZuAbwasser = atoi(value);
    }
    if (MATCH("h2o", "maxLiterAbwasserKanister")) {
        pconfig->h2o.maxLiterAbwasserKanister = atof(value);
    }
    if (MATCH("h2o", "reinigungszeitInSekunden")) {
        pconfig->h2o.reinigungszeitInSekunden = atoi(value);
    } else {
        return 0;
    }
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
    if (answerchange == 'Y' || answerchange == 'y' || answerchange == 'J' || answerchange == 'j') {
        sprintf(command, "sh /Energiebox/h2o/setIni.sh %f %f %d %d %f %d %d %f %d", aktuellesGesameltesAbwasser, 0.f, pumpeRelaisNr, gpd, warnLimitAbFilterMenge,
                filterZeitFuerNullKommaEinsLiterInSekunden, faktorGefiltertZuAbwasser, maxLiterAbwasserKanister, reinigungszeitInSekunden);
        system(command);
    }
}

// Ersetzt Zeichen mit anderem in einem String
void replace_char(char* s, char find, char replace) {
    while (*s != 0) {
        if (*s == find) *s = replace;
        s++;
    }
    // Usage: replace_char (strname, ' ' , '-');
}

// Programm Setup aufrufen und Filtereigenschaften abfragen
void setup() {
    configuration config;
    // Konfiguration von h2o in config laden
    if (ini_parse("/Energiebox/h2o/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/h2o/config.ini'\n");
    }
    char answersetup;
    printf("\n-> Einrichtung starten um neue Filteranlage zu konfigurieren? Y/N: ");
    scanf("%c", &answersetup);
    if (answersetup == 'Y' || answersetup == 'y' || answersetup == 'J' || answersetup == 'j') {
        // Wenn ja, alle Daten zusammen erfragen und externe Datei aufrufen
        int p1;
        printf("-> An welchem Relais vom 12V Block (1 bis 16) sind die Wasserpumpen angeschlossen?: ");
        scanf("%d", &p1);
        int p2;
        printf("-> Wieviel GPD hat die Filteranlage? (etc. 50, 75, 100): ");
        scanf("%d", &p2);
        int p3;
        printf("-> Wieviel (fertige) Liter Wasser sollte maximal mit einem Satz Filter gefiltert werden?: ");
        scanf("%d", &p3);
        literProTag = literProGalone * gpd;
        int p4 = 0.1f / (((literProTag / 24) / 60) / 60);
        int p5;
        printf("-> Wie ist das Verhältnis von gefiltertem Wasser zu Abwasser/Spülwasser? 1 zu: ");
        scanf("%d", &p5);
        int p6;
        printf("-> Wieviel Liter kann der Abwasser/Spülwasser Tank aufnehmen?: ");
        scanf("%d", &p6);
        int p7;
        printf("-> Wieviele Sekunden sollen bei einer Spüllung gespüllt werden?: ");
        scanf("%d", &p7);
        sprintf(command, "sh /Energiebox/h2o/setIni.sh %f %f %d %d %f %d %d %f %d", 0.f, 0.f, p1, p2, (float)p3, p4, p5, (float)p6, p7);
        system(command);
    }
}

// Statistik ausgeben
void printStatistik() {
    printf("\n +----------------------------------------------+\n");
    printf(" |                   STATISTIK                  |\n");
    printf(" +----------------------------------------------+\n");
    printf(" | Filter                                       |\n");
    printf(" |  Leistung:\t\t\t\t %d GPD\t|\n", gpd);
    printf(" |  Faktor (Filtermenge/Abwasser):\t 1/%d \t|\n", faktorGefiltertZuAbwasser);
    printf(" |  Bisherige Filtermenge:\t\t %04.f L\t|\n", gesamteFilterMengeInLiter);
    printf(" |  Filterlimit für Filtertausch:\t %04.f L\t|\n", warnLimitAbFilterMenge);
    printf(" |  Restmenge bis Filtertausch:\t\t %04.f L\t|\n", warnLimitAbFilterMenge - gesamteFilterMengeInLiter);
    printf(" |  Spülzeit in Sekunden:\t\t %d\t|\n", reinigungszeitInSekunden);
    printf(" +----------------------------------------------+\n");
    printf(" | Tank                                         |\n");
    printf(" |  Fassungsvermögen:\t\t\t %04.f L\t|\n", maxLiterAbwasserKanister);
    printf(" |  Aktueller Füllstand:\t\t %04.f L\t|\n", aktuellesGesameltesAbwasser);
    printf(" |  Frei in Litern:\t\t\t %04.f L\t|\n", (maxLiterAbwasserKanister - aktuellesGesameltesAbwasser));
    printf(" +----------------------------------------------+\n");
    printf(" | Pumpe                                        |\n");
    printf(" |  Relais Numer von Pumpe:\t\t %d\t|\n", pumpeRelaisNr);
    printf(" +----------------------------------------------+\n\n");
}

// Anlage Reinigen per Durchspüllung (Parameter -clean)
void clearSystem() {
    configuration config;
    // Konfiguration von config.ini in config laden
    if (ini_parse("/Energiebox/h2o/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/h2o/config.ini'\n");
    }
    // Menge Abwasser + bereits gesameltes Abwasser berechnen und prüfen ob es weniger als maximaler Tankinhalt ist
    float ltr = ((float)reinigungszeitInSekunden / (float)filterZeitFuerNullKommaEinsLiterInSekunden);
    abwasserMenge = (ltr * 0.1) * faktorGefiltertZuAbwasser;
    // Prüfen ob generierte Abwassermenge + Tankinhalt nicht mehr als maximale füllmenge vom Tank ist
    if ((abwasserMenge + aktuellesGesameltesAbwasser) <= maxLiterAbwasserKanister) {
        // Fragen ob das Ventil gedreht wurde
        char answerclear;
        printf("\n\n -> Ventil auf Spülung stellen und Vorgang starten? J/N: ");
        scanf("%c", &answerclear);
        if (answerclear == 'Y' || answerclear == 'y' || answerclear == 'J' || answerclear == 'j') {
            // Wenn ja, Spülen
            // Reinigungszeit berechnen
            int s = reinigungszeitInSekunden;
            int stunden = s / 3600;
            s = s % 3600;
            int minuten = s / 60;
            s = s % 60;
            int sekunden = s;
            printf(" -> Die Reinigung wird jetzt gestartet. Geschätzte Zeit: %02d:%02d:%02d\n", stunden, minuten, sekunden);
            // filter einschalten
            sprintf(command, "12V %d 1 1", pumpeRelaisNr);
            system(command);
            int laufzeitSekunden = 0;
            while (true) {
                print_progress(laufzeitSekunden, reinigungszeitInSekunden);
                sleep(1);
                laufzeitSekunden++;
                if (laufzeitSekunden == reinigungszeitInSekunden) {
                    print_progress(reinigungszeitInSekunden, reinigungszeitInSekunden);
                    break;
                }
            }
            sprintf(command, "12V %d 0 %d", pumpeRelaisNr, 1);
            system(command);
            printf("\n -> Reinigung beendet!\n");
            // Neue Gesammelt Abwassermenge in Konfiguration speichern
            sprintf(command, "sh /Energiebox/h2o/setIni.sh %f %f %d %d %f %d %d %f %d", (abwasserMenge + aktuellesGesameltesAbwasser), (gesamteFilterMengeInLiter), pumpeRelaisNr,
                    gpd, warnLimitAbFilterMenge, filterZeitFuerNullKommaEinsLiterInSekunden, faktorGefiltertZuAbwasser, maxLiterAbwasserKanister, reinigungszeitInSekunden);
            system(command);
            printf(" -> Ventil auf normal Betrieb umstellen!\n\n");
        }
    } else {
        // Abwasser Kannister hat nicht genug freies Volumen für geplante Filterung. Programm beenden
        printf("\e[0;31m -> Wassertank ist zu voll. Spülung wird abgebrochen!\n    Zuerst Abwasser Tank entleeren mit h2o -empty!\n\n\e[0m");
    }
}

// Progress Bar Reinigung
void print_progress(size_t count, size_t max) {
    const int bar_width = 53;
    float progress = (float)count / max;
    int bar_length = progress * bar_width;
    printf("\r -> [");
    for (int i = 0; i < bar_length; ++i) {
        printf("#");
    }
    for (int i = bar_length; i < bar_width; ++i) {
        printf(" ");
    }
    printf("] %.2f%%", progress * 100);
    fflush(stdout);
}

// Progress Bar Filterung
void print_progress2(size_t count, size_t max) {
    const int bar_width = 38;
    float progress = (float)count / max;
    int bar_length = progress * bar_width;
    printf("\r -> [");
    for (int i = 0; i < bar_length; ++i) {
        printf("#");
    }
    for (int i = bar_length; i < bar_width; ++i) {
        printf(" ");
    }
    printf("] %.2f%%", progress * 100);
    fflush(stdout);
}

// Abwasser Kanister Literzähler auf 0 setzen (Parameter -empty)
void abwasserZaehlerReset() {
    // Abfragen ob Eimer gelert worden ist.
    char answer;
    printf("\n\n -> Wurde der Abwasser Tank entleert? Y/N: ");
    scanf("%c", &answer);
    if (answer == 'Y' || answer == 'y' || answer == 'J' || answer == 'j') {
        // Konfiguration laden
        configuration config;
        // Konfiguration von h2o in config laden
        if (ini_parse("/Energiebox/h2o/config.ini", handler, &config) < 0) {
            printf("Can't load '/Energiebox/h2o/config.ini'\n");
        }
        gesamteFilterMengeInLiter = config.h2o.gesamteFilterMengeInLiter;
        // Wenn ja, ini auf 0 setzen
        sprintf(command, "sh /Energiebox/h2o/setIni.sh %f %f %d %d %f %d %d %f %d", 0.f, gesamteFilterMengeInLiter, pumpeRelaisNr, gpd, warnLimitAbFilterMenge,
                filterZeitFuerNullKommaEinsLiterInSekunden, faktorGefiltertZuAbwasser, maxLiterAbwasserKanister, reinigungszeitInSekunden);
        system(command);
        printf(" -> Abwasser Tank auf 0 Liter eingestellt!\n\n");
    }
}

// Zeigt die Hilfe an
void showHelp() {
    printf("\n\e[0;31m Tool für automatische Filtersteuerung. Anwendungsbeispiel:\n\n");
    printf("  h2o 0.2     [Produziert 0,2 Liter gefiltertes Wasser]\n");
    printf("  h2o -clean  [Spült die Membrane]\n");
    printf("  h2o -change [Filter austauschen und Zähler auf 0 setzen]\n");
    printf("  h2o -empty  [Setzt Tank für Abwasser auf 0 Liter]\n");
    printf("  h2o -stats  [Zeigt Statistik über Filter und Tank]\n");
    printf("  h2o -setup  [Filter konfigurieren]\n\n");
}

void showLogo() {
    printf("\n\n   _   _ ____   ___     \n");
    printf("  | | | |___ \\ / _ \\  \n");
    printf("  | |_| | __) | | | | \n");
    printf("  |  _  |/ __/| |_| | \n");
    printf("  |_| |_|_____|\\___/  \n");
    printf("\n");
}
