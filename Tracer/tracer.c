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
#include <unistd.h>
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
	const char* filename = "/Energiebox/Tracer/tracer.txt";
	char batt_voltage[] = "Batterie: Aktuelle Spannung in Volt = ";
	FILE* input_file = fopen(filename, "r");
	if (!input_file) exit(EXIT_FAILURE);
	char* contents = NULL;
	size_t len = 0;
	// Jede Zeile auslesen
	while (getline(&contents, &len, input_file) != -1) {
		//printf("%s", contents);
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
			if(config.grid.battVoltageStartLoading <= atof(token)){
				//printf("atof(token) %2.2f", atof(token));
				//printf("config.grid.battVoltageStartLoading %2.2f", config.grid.battVoltageStartLoading);
			}
			else {
				//printf("-> Niedrige Batteriespannung entdeckt, Grid load wird gestartet! Es werden %d Wh geladen\n", config.grid.loadingCapacityWh);
				
				
if (access("/Energiebox/Grid/isLoading.lock", F_OK) == 0) {
    fprint("ja");
} else {
    fprint("no");
}
				
				
				sprintf(command, "/Energiebox/Grid/grid -w %d & ", config.grid.loadingCapacityWh);
				system(command);
				
				sprintf(command, "touch /Energiebox/Grid/isLoading.lock");
                system(command);

				
				
			}
		}
	}
	fclose(input_file);
	free(contents);
	exit(EXIT_SUCCESS);
}
