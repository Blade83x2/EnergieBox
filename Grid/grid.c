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
#include "iniparse.h"
#include <unistd.h> // sleep()
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>

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
char command[100];

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

// Schreibt Bit für Relaiszustand
void setBit(int Port, int Status) {
    int Get_Port, PIN;
    if (Port > -1 && Port < 8) {
        Get_Port = mcp_readRegister(0x12);
        PIN = Port;
    } else {
        Get_Port = mcp_readRegister(0x13);
        PIN = Port % 8;
    }
    if (Status == 1) {
        Get_Port |= (1 << PIN);
    } else {
        Get_Port &= (~(1 << PIN));
    }
    if (Port > -1 && Port < 8) {
        mcp_writeRegister( 0x12, Get_Port);
    } else {
        mcp_writeRegister(0x13, Get_Port);
    }
}

// Zeigt Hilfe auf Console an
int showHelp(char**argv, void* config) {
    printf("\n\e[0;31m Falsche Parameter! Beispiel:\n\n");
    printf("  %s -w 200\t\t[lädt 200 Wattstunden aus dem Netz in die Batterie]\n", argv[0]);
    printf("  %s -s 50\t\t[beendet den aktuellen Ladevorgang in 50 Sekunden]\n", argv[0]);
    printf("  %s -h\t\t[zeigt diese Hilfe an]\n\n\n", argv[0]);
    return -1;
}


bool isNumeric(const char *str) {
    while(*str != '\0') {
        if(*str < '0' || *str > '9')
            return false;
        str++;
    }
    return true;
}

// Programmstart
int main(int argc, char *argv[]){ 
    configuration config;
    if (ini_parse("/Energiebox/Grid/config.ini", handler, &config) < 0) { fprintf(stderr, "Can't load '/Energiebox/Grid/config.ini\n"); return 1; }
    if(wiringPiSetup()<0) { fprintf(stderr, "wiringPiSetup error!!!\n"); return -1; }
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) { fprintf(stderr, "wiringPi I2C Setup error!!!"); return -1; }
    if(argc == 1) {
        // Keine Parameterübergabe. Hilfe anzeigen
        return showHelp(argv, &config);
    }
    
    float supplyMaxVoltage = 0.2f;
    supplyMaxVoltage = config.grid.supplyMaxVoltage;
    float supplyMaxCurrent = 0.2f;
    supplyMaxCurrent = config.grid.supplyMaxCurrent;
    float supplyLoadPower = 0.2f;
    supplyLoadPower = supplyMaxVoltage * supplyMaxCurrent;
    float supplyLoadTimeSec = 0.f;
    float supplyLoadWattStunden = 0.f;

    int opt; 
    while((opt = getopt(argc, argv, "hs:w:")) != -1)  {  
        switch(opt)  {  
            case 'h':  
                return showHelp(argv, &config);
                break;  
            case 's':
                if(!isNumeric(optarg) ){
                    printf("\e[0;31m Der eingegebene Parameter ist keine Zahl!\n");
                    return 1; 
                }
                sleep(atof(optarg));
                setBit(1, 1); // Batterie Relais ausschalten 
                sleep(5);
                setBit(0, 1); // Netzanschluss Relais ausschalten 
                // beim starten sowie beim runter fahren ausführen
                sprintf(command, "rm -f /Energiebox/Grid/isLoading.lock");
                system(command);
                return 0;
                break;  
            case 'w': 
                supplyLoadWattStunden = atof(optarg);
                if(!isNumeric(optarg) ){
                    printf("\e[0;31m Der eingegebene Parameter ist keine Zahl!\n");
                    return 1; 
                }
                if(supplyLoadWattStunden < config.grid.supplyMinLoadWh){
                    printf("\e[0;31m Der minimale Wert für -w beträgt %d\n", config.grid.supplyMinLoadWh);
                    return 1;
                }
                if(supplyLoadWattStunden > config.grid.supplyMaxLoadWh){
                    printf("\e[0;31m Der maximale Wert für -w beträgt %d\n", config.grid.supplyMaxLoadWh);
                    return 1;
                }
                printf("\n  Ladespannung (U):\t\t %2.2fV\n", supplyMaxVoltage);
                printf("  Ladestrom (A):\t\t %3.2fA\n", supplyMaxCurrent);
                printf("  Ladeleistung pro Stunde (W):\t %4.2fW \n", supplyLoadPower);
                printf("  Ladeleistung pro Sekunde (W):\t %4.2fW \n", supplyLoadPower/3600);
                supplyLoadTimeSec = supplyLoadWattStunden / (supplyLoadPower/3600);
                printf("  Ladezeit in Sekunden:\t\t %4.0f Sek\n\n", supplyLoadTimeSec);  
                // prüfen ob bereits schon eine ladung am laufen ist
				if (access("/Energiebox/Grid/isLoading.lock", F_OK) == 0) {
                    printf("\e[0;31m Es wird bereits aus dem Netz geladen!\n\n");
				} 
				else 
                {
                    setBit(0, 0); // Netzanschluss Relais einschalten 
                    sleep(5); // Zeitabstand für Transformationsfeld aufzubauen
                    setBit(1, 0); // Batterie Relais einschalten 
					sprintf(command, "touch /Energiebox/Grid/isLoading.lock");
					system(command);            
                    // Selbst aufrufen mit Parameter -s sowie Ausschaltzeit in Sekunden
                    sprintf(command, "%s -s %4.0f & ", argv[0], supplyLoadTimeSec);
                    system(command);
				}
                return 0;
                break;  
            case ':':  
                printf("Dieser Parameter benötigt einen Wert!\n");  
                return 1;
                break;  
            case '?':  
                return showHelp(argv, &config);
                break;  
        }  
    }  
    return 0;
}
