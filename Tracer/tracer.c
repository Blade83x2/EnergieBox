#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


int main(void) {
	const char* filename = "/Energiebox/Tracer/tracer.txt";
	char command[255];
	char batt_voltage[] = "Batterie: Aktuelle Spannung in Volt = ";
	char batt_volatage_disable[] = "48.00"; // in Volt
	FILE* input_file = fopen(filename, "r");
	if (!input_file) exit(EXIT_FAILURE);
	char* contents = NULL;
	size_t len = 0;
	// Jede Zeile auslesen
	while (getline(&contents, &len, input_file) != -1) {
		printf("%s", contents);
		// n Ersten Zeichen vergleichen mit batt_voltage
		if(strncmp(batt_voltage, contents, 38)==0){
			//printf("vergleich\n");
			// Delimeter auf = setzen
			char *token = strtok(contents, "=");
			// Position auf Bereich hinter = schieben
			token = strtok(NULL, "=");
			// Leerzeichen hinter = entfernen
			for(int n=0; n < strlen(token); ++n){
				token[n] = token[n+1];
			}
			// hinter der zweiten Komastelle abschneiden für float vergleich
			token[5] = '\0';
			//printf("Cur. batt: %f \n disable at:  %f \n", atof(token), atof(batt_volatage_disable));
			if(atof(batt_volatage_disable) <= atof(token)){
				//printf("volt ok \n");
			}
			else {
				printf("-> Niedrige Batteriespannung entdeckt, System Powersave Modus wird eingeschaltet!\n");
				// Alles ausschalten was nicht notwendig ist
				// 230V
				sprintf(command, "/Energiebox/230V/230V 1 0 0 && /Energiebox/230V/230V 2 0 0 && /Energiebox/230V/230V 3 0 0 && /Energiebox/230V/230V 4 0 0 && /Energiebox/230V/230V 5 0 0");
				system(command);
				sprintf(command, "/Energiebox/230V/230V 6 0 0 && /Energiebox/230V/230V 7 0 0 && /Energiebox/230V/230V 8 0 0 && /Energiebox/230V/230V 9 0 0 && /Energiebox/230V/230V 10 0 0");
				system(command);
				sprintf(command, "/Energiebox/230V/230V 11 0 0 && /Energiebox/230V/230V 12 0 0 && /Energiebox/230V/230V 13 0 0 && /Energiebox/230V/230V 14 0 0 && /Energiebox/230V/230V 15 0 0 && /Energiebox/230V/230V 16 0 0");
				system(command);

				// 12V
				sprintf(command, "/Energiebox/12V/12V 3 0 0 && /Energiebox/12V/12V 4 0 0 && /Energiebox/12V/12V 5 0 0 && /Energiebox/12V/12V 6 0 0 && /Energiebox/12V/12V 7 0 0");
				system(command);
				sprintf(command, "/Energiebox/12V/12V 10 0 0 && /Energiebox/12V/12V 11 0 0 && /Energiebox/12V/12V 12 0 0 && /Energiebox/12V/12V 13 0 0 && /Energiebox/12V/12V 14 0 0 && /Energiebox/12V/12V 15 0 0");
				system(command);
			}
		}
	}
	fclose(input_file);
	free(contents);
	exit(EXIT_SUCCESS);
}
