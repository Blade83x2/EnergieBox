#include <stdio.h>
#include <stdlib.h> // atoi()
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mymcp23017.h"
#include <unistd.h> // sleep()
#include <string.h>
#include <stdbool.h> 
#include <ctype.h>

// Kompilieren mit: sudo make
// Git: sudo git add -A && sudo git commit -m "Parameter Validierung hinzugefügt" && sudo git push -u origin main

int getBit(int Port);
void setBit(int Port, int Status);
bool checkMainParameter(char* paramName, int number, void* config);
int showHelp(char**argv, void* config);

// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
    float maxAmpere;
    float maxMicroControllerAmpere;
} mcp_setup;

// Relais Struktur
typedef struct {
    const char* name;
    bool activateOnStart;
    float aMax;
} relais_config;

typedef struct {
    mcp_setup mcp;
    relais_config r1;
    relais_config r2;
    relais_config r3;
    relais_config r4;
    relais_config r5;
    relais_config r6;
    relais_config r7;
    relais_config r8;
    relais_config r9;
    relais_config r10;
    relais_config r11;
    relais_config r12;
    relais_config r13;
    relais_config r14;
    relais_config r15;
    relais_config r16;
} configuration;

char deviceNames[16][40];
char deviceActiveOnStart[16][6];





static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("mcp", "address")) {  pconfig->mcp.address = atoi(value); } 
    else if(MATCH("mcp", "numberOfRelaisActive")) { pconfig->mcp.numberOfRelaisActive = atoi(value); } 
    else if(MATCH("Relais 1", "name")) { strcpy(deviceNames[0], strdup(value)); } 
    else if(MATCH("Relais 1", "activateOnStart")) { strcpy(deviceActiveOnStart[0], value); } 
    else if(MATCH("Relais 2", "name")) { strcpy(deviceNames[1], strdup(value)); } 
    else if(MATCH("Relais 2", "activateOnStart")) { strcpy(deviceActiveOnStart[1], value); } 
    else if(MATCH("Relais 3", "name")) { strcpy(deviceNames[2], strdup(value)); } 
    else if(MATCH("Relais 3", "activateOnStart")) { strcpy(deviceActiveOnStart[2], value); } 
    else if(MATCH("Relais 4", "name")) { strcpy(deviceNames[3], strdup(value)); } 
    else if(MATCH("Relais 4", "activateOnStart")) {  strcpy(deviceActiveOnStart[3], value);  } 
    else if(MATCH("Relais 5", "name")) { strcpy(deviceNames[4], strdup(value)); } 
    else if(MATCH("Relais 5", "activateOnStart")) { strcpy(deviceActiveOnStart[4], value);  } 
    else if(MATCH("Relais 6", "name")) { strcpy(deviceNames[5], strdup(value)); } 
    else if(MATCH("Relais 6", "activateOnStart")) { strcpy(deviceActiveOnStart[5], value);  } 
    else if(MATCH("Relais 7", "name")) { strcpy(deviceNames[6], strdup(value)); } 
    else if(MATCH("Relais 7", "activateOnStart")) { strcpy(deviceActiveOnStart[6], value); } 
    else if(MATCH("Relais 8", "name")) { strcpy(deviceNames[7], strdup(value)); } 
    else if(MATCH("Relais 8", "activateOnStart")) { strcpy(deviceActiveOnStart[7], value);  } 
    else if(MATCH("Relais 9", "name")) { strcpy(deviceNames[8], strdup(value)); } 
    else if(MATCH("Relais 9", "activateOnStart")) { strcpy(deviceActiveOnStart[8], value);  } 
    else if(MATCH("Relais 10", "name")) { strcpy(deviceNames[9], strdup(value)); } 
    else if(MATCH("Relais 10", "activateOnStart")) {  strcpy(deviceActiveOnStart[9], value);  } 
    else if(MATCH("Relais 11", "name")) { strcpy(deviceNames[10], strdup(value)); } 
    else if(MATCH("Relais 11", "activateOnStart")) {  strcpy(deviceActiveOnStart[10], value);  } 
    else if(MATCH("Relais 12", "name")) { strcpy(deviceNames[11], strdup(value)); } 
    else if(MATCH("Relais 12", "activateOnStart")) {  strcpy(deviceActiveOnStart[11], value);  } 
    else if(MATCH("Relais 13", "name")) { strcpy(deviceNames[12], strdup(value)); } 
    else if(MATCH("Relais 13", "activateOnStart")) {  strcpy(deviceActiveOnStart[12], value);  } 
    else if(MATCH("Relais 14", "name")) { strcpy(deviceNames[13], strdup(value)); } 
    else if(MATCH("Relais 14", "activateOnStart")) {  strcpy(deviceActiveOnStart[13], value);  } 
    else if(MATCH("Relais 15", "name")) { strcpy(deviceNames[14], strdup(value)); } 
    else if(MATCH("Relais 15", "activateOnStart")) {  strcpy(deviceActiveOnStart[14], value);  } 
    else if(MATCH("Relais 16", "name")) { strcpy(deviceNames[15], strdup(value)); } 
    else if(MATCH("Relais 16", "activateOnStart")) {  strcpy(deviceActiveOnStart[15], value);  } 
    else { return 0; }
    return 1;
}

int main(int argc, char**argv) { 
    configuration config;
    if (ini_parse("/Energiebox/12V/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/12V/config.ini'\n");
        return 1;
    }
    if(wiringPiSetup()<0) {
        printf("wiringPiSetup error!!!");
        return -1;
    }
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) {
        printf("wiringPi I2C Setup error!!!");
        return -1;
    }
    if(argc == 1) {
        // Keine Parameterübergabe. Liste anzeigen was geschaltet ist
        printf("\n\e[0;34m\e[43m Rel.\tState\t  Name                 \e[0m\n");

	// TODO SCHALTZUSTAND aus config lesen


        for(int x=1; x<=config.mcp.numberOfRelaisActive; x++) {
            printf("\e[0;36m  %d\t%s %s\t  %s\e[0m\n", x, (getBit(x)==0?"\e[0;31m":"\e[0;32m"), ((getBit(x)==0)?"Off":"On"), deviceNames[x-1]);
        }
        printf("\n");
    }
    else if(argc == 2) {
        if(!checkMainParameter("relaisNumber", atoi(argv[1]), &config)) {
            return showHelp(argv, &config);
        }
        printf("%d", getBit(atoi(argv[1]))); // Nur Relais Nr. übergeben. Bit auslesen ob gesetzt oder nicht und integer printen
    }
    else if(argc == 3) {
        if(!checkMainParameter("relaisNumber", atoi(argv[1]), &config) || !checkMainParameter("relaisZustand", atoi(argv[2]), &config)) {
            return showHelp(argv, &config);
        }
	// TODO BIT kurz setzen und Relais wieder aus machen
	// TODO config eltakoState setzen

        setBit(atoi(argv[1])-1, atoi(argv[2])==1?0:1); // Relais Nr. & Zustand übergeben. Bit setzen
    }
    else if(argc == 4) {
        if(!checkMainParameter("relaisNumber", atoi(argv[1]), &config) || !checkMainParameter("relaisZustand", atoi(argv[2]), &config) || !checkMainParameter("relaisTime", atoi(argv[3]), &config )) {
            return showHelp(argv, &config);
        }
        sleep(atoi(argv[3]) * 60); // Relais Nr., Zustand & Schaltzeit in Minuten übergeben. 

        setBit(atoi(argv[1])-1, atoi(argv[2])==1?0:1);
    }
    else {
        return showHelp(argv, &config);
    }
    return 0;
}

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

int getBit(int Port) {
    Port = Port - 1;
    int Get_Port, PIN;
    if (Port > -1 && Port < 8) {
        Get_Port = mcp_readRegister(0x12);
        PIN = Port;
    } else {
        Get_Port = mcp_readRegister(0x13);
        PIN = Port % 8;
    }
    if(Get_Port & (1 << PIN)) {
        return 0;
    }
    else {
        return 1;
    }
}

bool checkMainParameter(char* paramName, int number, void* config) {
    configuration* pconfig = (configuration*)config;
    if (strcmp(paramName, "relaisNumber") == 0)  {
        if ( number < 1 || number > pconfig->mcp.numberOfRelaisActive )
            return false;
    } 
    else if (strcmp(paramName, "relaisZustand") == 0) {
        if ( number < 0 || number > 1 )
            return false;
    } 
    else if (strcmp(paramName, "relaisTime") == 0) {
        if ( number < 1 || number > 86400000 )
            return false;
    } 
    return true;
}

int showHelp(char**argv, void* config) {
    configuration* pconfig = (configuration*)config;
    printf("\n\e[0;31m Falsche Parameter! Beispiel:\n\n");
    printf("  %s\t\t\t[zeigt denn aktuellen Belegungsplan an]\n", argv[0]);
    printf("  %s 5\t\t\t[gibt denn aktuellen Schaltzustand von Relais 5 zurück. Relais verfügbar: 1 bis %d)]\n", argv[0], pconfig->mcp.numberOfRelaisActive);
    printf("  %s 5 1\t\t[schaltet Relais 5 auf 1 (an)]\n", argv[0]);
    printf("  %s 7 0 10\t\t[schaltet Relais 7 aus in 10 Minuten]\n", argv[0]);
    printf("  %s 10 1 5 & disown\t[schaltet Relais 10 im Hintergrund an in 5 Minuten und gibt die Konsole frei]\n\n", argv[0]);
    printf("  In der Konfigurationsdatei können Namen für denn Belegungsplan vergeben werden!\n");
    printf("  /Energiebox/%s/config.ini\e[0m \n\n", argv[0]);
    return -1;
}
