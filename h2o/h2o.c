#include <stdio.h>
#include <unistd.h> // sleep()
#include <string.h>
#include <stdbool.h> 
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>


int gpd=50;   // Galone per Day angeben von der Osmose Anlage um Wassermenge zu berechnen
float oneGalloneInLiter=3.785;
float literPerDay;
float literPerHour;
float literPerMinute;


int runtime=0;  // laufzeit minuten vom abwasser

void showHelp() {
    printf("\n\e[0;31m Falsche Parameter! Beispiel:\n\n");
    printf(" h2o -l x [Produziert x Liter Wasser]\n\n\e[0m");
}


int main(int argc, char**argv) { 
    printf("\033[2J\033[1;1H"); // clear screen
    system("12V 6 0"); // disable relais
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
    printf("\t\\_/  \\_\\____\\_/ \\____\\_/\\_\\  \\____/_/  \\____/ \\_/ \\____\\_/  \\| \n");
                                                                  
    if(argc == 3) {
        const char * liter=argv[2];

        
        printf("\n\n\tes werden %s liter produziert\n\n", liter);
        
        printf("literPerDay: %2f\n", literPerDay=gpd*oneGalloneInLiter);

        
        return 0;
    }
    showHelp();
}

