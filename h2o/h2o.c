#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

// Galonen pro Tag Filtermenge
int gpd = 50;

// Maximale Abwasser Literanzahl bei Reinigung
int maxLiterAbwasserBeiReinigung = 5;

// maxLiterAbwasserKanister (Überlaufschutz)
int maxLiterAbwasserKanister = 10;

// Aktuell gesameltes Abwasser (wird bei param1 = -r auf Null gesetzt)
int aktuellesGesameltesAbwasser = 0;

// Relaise Nr. für Pumpe & Boosterpumpe
int pumpeRelaisNr = 6;  

// Liter Pro Galone
float literProGalone = 3.7854f;

// Liter pro Tag (gpd * literProGalone). wird in main berechnet
float literProTag = 0.f;

// Maximale Abwasser Literanzahl bei Reinigung
//int maxLiterAbwasserBeiReinigung = 5;

// maxLiterAbwasserKanister (Überlaufschutz)
float maxLiterAbwasserKanister = 10f;

// Aktuell gesameltes Abwasser (wird bei param1 = -r auf Null gesetzt)
float aktuellesGesameltesAbwasser = 0f;   //TODO aus ini lesen !!!!!!!!!!!!!

// Filtermenge (wird von param1 überschrieben)
float filterMenge = 0.f;

// Funktionen deklarieren
void clearSystem();
void showHelp();
void showLogo();
void abwasserZaehlerReset();

// Programmstart
int main(int argc, char* argv[]) { 
    printf("\033[2J\033[1;1H"); // Bildschirm löschen
    system("12V $pumpeRelaisNr 0"); // Wenn Pumpe an, dann ausschalten
    showLogo();


    printf("GPD: %d\n", gpd);
    printf("Liter pro Galone %f\n", literProGalone);
    printf("Liter pro Tag: %f \n\n\n", literProGalone*gpd);



    if(argc == 1) {
        // Keine Parameter übergeben,Hilfe anzeigen
        showHelp();
        return 0;
    }
    // prüfen ob argv[1] -c oder -r oder -l enthält, wenn nicht, Hilfe anzeigen
    const char * param1 = argv[1];
    if ( (strcmp(param1, "-c") == 1) || (strcmp(param1, "-r") == 1) || (strcmp(param1, "-l") == 1)) {
        showHelp();
        return 0;
    }


    // wenn ein parameter übergeben worden ist
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
        // Wenn param1 gleich -l ist
        if (strcmp(param1, "-l") == 0) {
            // und param2 größer als 0.1 ist
            filterMenge = (float) atof(argv[2]);
            if (filterMenge >= 0.1){
                printf(" Wasser wird gefiltert:\n ");
                printf(" Filtermenge  = %f Liter\n", filterMenge);






                // TODO filterzeit ermitteln und pumpe für diese zeit anstellen







            } else {
                printf("Die minimale Wassermenge zum filtern muss mindestens 0.1 Liter betragen!\n\n");
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

    // TODO zuerst eimer leer machen, dann bestätigen, dann reinigen


}

// Abwasser Kanister Literzähler auf 0 setzen
void abwasserZaehlerReset() {
    printf("\nAbwasser Reset\n\n");
    // Abfragen ob Eimer gelert worden ist





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
