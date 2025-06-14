#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mymcp23017.h"
#include "iniparse.h"
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h> // atoi()


// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
} mcp_setup;

typedef struct {
    mcp_setup mcp;
} configuration;

static int handlerGrid(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("mcp", "address")) {  pconfig->mcp.address = atoi(value); } 
    else if(MATCH("mcp", "numberOfRelaisActive")) { pconfig->mcp.numberOfRelaisActive = atoi(value); } 
    else { return 0; }
    return 1;
}

static int handler12(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("mcp", "address")) {  pconfig->mcp.address = atoi(value); } 
    else if(MATCH("mcp", "numberOfRelaisActive")) { pconfig->mcp.numberOfRelaisActive = atoi(value); } 
    else { return 0; }
    return 1;
}

static int handler230(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("mcp", "address")) {  pconfig->mcp.address = atoi(value); } 
    else if(MATCH("mcp", "numberOfRelaisActive")) { pconfig->mcp.numberOfRelaisActive = atoi(value); } 
    else { return 0; }
    return 1;
}

// System Kommandos String
char command[100];

int main(int argc, char**argv) {
    configuration config;
    /////////////////////
    //// 12 Volt  ///////
    /////////////////////
    // Konfiguration von 12V in config laden
    if (ini_parse("/Energiebox/12V/config.ini", handler12, &config) < 0) {
        fprintf (stderr, "Can't load '/Energiebox/230V/config.ini\n") ;
        exit (EXIT_FAILURE) ;
    }
    // Wirig PI prüfen
    if(wiringPiSetup()<0) {
        fprintf (stderr, "wiringPiSetup error\n") ;
        exit (EXIT_FAILURE) ;
    }
    // MCP  Portexpander
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) {
        fprintf (stderr, "wiringPi I2C Setup error\n") ;
        exit (EXIT_FAILURE) ;
    }
    // Register Bits
    mcp_initReg();
    // Alle als Output definieren und auf aus stellen
    for(int i = 0; i<config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i,1);
        sprintf(command, "bash /Energiebox/12V/setIni.sh %d %d", (i+1), 0);
        system(command);
        sleep(0.1);
    }

    
    ////////////////////
    ///// 230 Volt  ////
    ////////////////////
    // config Objekt überladen
    if (ini_parse("/Energiebox/230V/config.ini", handler230, &config) < 0) {
        fprintf (stderr, "Can't load '/Energiebox/230V/config.ini\n") ;
        exit (EXIT_FAILURE) ;
    }
    // Wirig PI prüfen
    if(wiringPiSetup()<0) {
        fprintf (stderr, "wiringPiSetup error\n") ;
        exit (EXIT_FAILURE) ;
    }
    // MCP  Portexpander
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) {
        fprintf (stderr, "wiringPi I2C Setup error\n") ;
        exit (EXIT_FAILURE) ;
    }
    mcp_initReg();
    // Alle als OUTPUT definieren und ausschalten
    for(int i = 0; i<config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i, 1);
        sprintf(command, "bash /Energiebox/230V/setIni.sh %d %d", (i+1), 0);
        system(command);
        sleep(0.1);
    }
    

    
    ////////////////////
    ///// GRID      ////
    ////////////////////
    // config Objekt überladen
    if (ini_parse("/Energiebox/Grid/config.ini", handlerGrid, &config) < 0) {
        fprintf (stderr, "Can't load '/Energiebox/Grid/config.ini\n") ;
        exit (EXIT_FAILURE) ;
    }
    // Wirig PI prüfen
    if(wiringPiSetup()<0) {
        fprintf (stderr, "wiringPiSetup error\n") ;
        exit (EXIT_FAILURE) ;
    }
    // MCP  Portexpander
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) {
        fprintf (stderr, "wiringPi I2C Setup error\n") ;
        exit (EXIT_FAILURE) ;
    }
    mcp_initReg();
    // Alle als OUTPUT definieren und ausschalten
    for(int i = 0; i<config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i, 1);
        sleep(0.1);
    }
    sprintf(command, "rm -f /Energiebox/Grid/isLoading.lock");
    system(command);
    return 0;
}
