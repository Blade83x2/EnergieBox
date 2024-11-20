/*
 * Controller & Management Software for Battery charging
 * Vendor: Johannes Krämer
 * Version: 1.0
 * Date: 20.11.2024
*/
#include <stdio.h>
#include <stdlib.h> // atoi()
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mymcp23017.h"
#include <unistd.h> // sleep()
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>



// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
} mcp_setup;

// GRID Setup
typedef struct {
    float supplyMaxCurrent;
    float supplyMaxVoltage;
    int supplyMinLoadWh;
    int supplyMaxLoadWh;
    float battVoltageStartLoading;
    int loadingCapacityWh;
} grid_setup;

// config
typedef struct {
    mcp_setup mcp;
    grid_setup grid;
} configuration;

// System Kommandos String
char command[255];

// Netzteilparameter in config.txt eintragen!
static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("mcp", "address")) {  pconfig->mcp.address = atoi(value); }
    else if(MATCH("mcp", "numberOfRelaisActive")) { pconfig->mcp.numberOfRelaisActive = atoi(value); }
    else if(MATCH("grid", "supplyMaxCurrent")) { pconfig->grid.supplyMaxCurrent = atof(value); }
    else if(MATCH("grid", "supplyMaxVoltage")) { pconfig->grid.supplyMaxVoltage = atof(value); }
    else if(MATCH("grid", "supplyMinLoadWh")) { pconfig->grid.supplyMinLoadWh = atoi(value); }
    else if(MATCH("grid", "supplyMaxLoadWh")) { pconfig->grid.supplyMaxLoadWh = atoi(value); }
    else if(MATCH("grid", "battVoltageStartLoading")) { pconfig->grid.battVoltageStartLoading = atof(value); }
    else if(MATCH("grid", "loadingCapacityWh")) { pconfig->grid.loadingCapacityWh = atoi(value); }
    else { return 0; }
    return 1;
}






int main(void) {
	
    configuration config;
    if (ini_parse("/Energiebox/Grid/config.ini", handler, &config) < 0) { fprintf(stderr, "Can't load '/Energiebox/Grid/config.ini\n"); return 1; }
	
	printf("-> %2.2fV \n", config.grid.battVoltageStartLoading);
	
	
	const char* filename = "/Energiebox/Tracer/tracer.txt";
	
	char batt_voltage[] = "Batterie: Aktuelle Spannung in Volt = ";
	char batt_volatage_disable[] = "50.00"; // in Volt
	FILE* input_file = fopen(filename, "r");
	if (!input_file) exit(EXIT_FAILURE);
	char* contents = NULL;
	size_t len = 0;
	// Jede Zeile auslesen
	while (getline(&contents, &len, input_file) != -1) {
		printf("%s", contents);
		// n Ersten Zeichen vergleichen mit batt_voltage
		if(strncmp(batt_voltage, contents, 38)==0){
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
			if(atof(batt_volatage_disable) <= atof(token)){
				//printf("volt ok \n");
			}
			else {
				printf("-> Niedrige Batteriespannung entdeckt, Grid load wird gestartet!\n");
				sprintf(command, "grid -w 100");
				system(command);
				// Alles ausschalten was nicht notwendig ist
				// 230V
				//sprintf(command, "/Energiebox/230V/230V 1 0 0 && /Energiebox/230V/230V 2 0 0 && /Energiebox/230V/230V 3 0 0 && /Energiebox/230V/230V 4 0 0 && /Energiebox/230V/230V 5 0 0");
				//system(command);
				//sprintf(command, "/Energiebox/230V/230V 6 0 0 && /Energiebox/230V/230V 7 0 0 && /Energiebox/230V/230V 8 0 0 && /Energiebox/230V/230V 9 0 0 && /Energiebox/230V/230V 10 0 0");
				//system(command);
				//sprintf(command, "/Energiebox/230V/230V 11 0 0 && /Energiebox/230V/230V 12 0 0 && /Energiebox/230V/230V 13 0 0 && /Energiebox/230V/230V 14 0 0 && /Energiebox/230V/230V 15 0 0 && /Energiebox/230V/230V 16 0 0");
				//system(command);

				// 12V
				//sprintf(command, "/Energiebox/12V/12V 3 0 0 && /Energiebox/12V/12V 4 0 0 && /Energiebox/12V/12V 5 0 0 && /Energiebox/12V/12V 6 0 0 && /Energiebox/12V/12V 7 0 0");
				//system(command);
				//sprintf(command, "/Energiebox/12V/12V 10 0 0 && /Energiebox/12V/12V 11 0 0 && /Energiebox/12V/12V 12 0 0 && /Energiebox/12V/12V 13 0 0 && /Energiebox/12V/12V 14 0 0 && /Energiebox/12V/12V 15 0 0");
				//system(command);
			}
		}
	}
	fclose(input_file);
	free(contents);
	exit(EXIT_SUCCESS);
}
