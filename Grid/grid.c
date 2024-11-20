#include <stdio.h>
#include <stdlib.h> // atoi()
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mymcp23017.h"
#include <unistd.h> // sleep()
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


// Funktionen vordeklarieren
int getBit(int Port);
void setBit(int Port, int Status);

int showHelp(char**argv, void* config);


// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
} mcp_setup;


// GRID Setup
typedef struct {
    int supplyMaxCurrent;
    int supplyMaxVoltage;
} grid_setup;



typedef struct {
    mcp_setup mcp;
    grid_setup grid;
} configuration;




static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("mcp", "address")) {  pconfig->mcp.address = atoi(value); }
    else if(MATCH("mcp", "numberOfRelaisActive")) { pconfig->mcp.numberOfRelaisActive = atoi(value); }
    else if(MATCH("grid", "supplyMaxCurrent")) { pconfig->grid.supplyMaxCurrent = atoi(value); }
    else if(MATCH("grid", "supplyMaxVoltage")) { pconfig->grid.supplyMaxVoltage = atoi(value); }
    else { return 0; }
    return 1;
}




// Programmstart
int main(int argc, char**argv) { 
    configuration config;
    if (ini_parse("/Energiebox/Grid/config.ini", handler, &config) < 0) { fprintf(stderr, "Can't load '/Energiebox/Grid/config.ini\n"); return 1; }
    if(wiringPiSetup()<0) { fprintf(stderr, "wiringPiSetup error!!!\n"); return -1; }
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) { fprintf(stderr, "wiringPi I2C Setup error!!!"); return -1; }

    

    
    
    mcp_digitalWrite(0, 0);
    mcp_digitalWrite(1, 1);


    sleep(2);
    return 0;
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

// Liesst Bit von Relais Zustand aus
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


// Zeigt Hilfe auf Console an
int showHelp(char**argv, void* config) {
    configuration* pconfig = (configuration*)config;
    printf("\n\e[0;31m Falsche Parameter! Beispiel:\n\n");
    printf("  %s\t\t\t[zeigt denn aktuellen Belegungsplan an]\n", argv[0]);
    printf("  %s 5\t\t\t[gibt denn aktuellen Schaltzustand von Relais 5 zurück. Relais verfügbar: 1 bis %d)]\n", argv[0], pconfig->mcp.numberOfRelaisActive);
    printf("  %s 5 1\t\t[schaltet Relais 5 auf 1 (an)]\n", argv[0]);
    printf("  %s 7 0 10\t\t[schaltet Relais 7 aus in 10 Sekunden]\n", argv[0]);
    printf("  %s 10 1 300 & disown\t[schaltet Relais 10 im Hintergrund an in 5 Minuten und gibt die Konsole frei]\n", argv[0]);
    printf("  %s -set 5\t\t[Ruft das Einstellungsmenü für Relais 5 auf]\e[0m \n\n", argv[0]);
    return -1;
}
