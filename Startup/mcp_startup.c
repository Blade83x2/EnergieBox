#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mymcp23017.h"
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h> 
#include <ctype.h>
#include <stdlib.h> // atoi()


// Kompilieren mit: sudo make
// Git: sudo git add -A && sudo git commit -m "Initial Version" && sudo git push -u origin main


// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
} mcp_setup;

// Relais Strukturen
typedef struct {
    const char* name;
    bool activateOnStart;
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
	time_t t;
    struct tm * ts;
    t = time(NULL);
    ts = localtime(&t);

    configuration config;
    if (ini_parse("/Energiebox/12V/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/12V/config.ini'\n");
        return 1;
    }
    
    if(wiringPiSetup()<0) {
        printf("wiringPiSetup error!!!");
        return -1;
    }
    // 12 Volt
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) {
        printf("wiringPiI2CSetup error!!!");
        return -1;
    }
    mcp_initReg();
    // Alle als OUTPUT definieren und ausschalten
    // mcp_writeRegister(0x00, 0x00); // bank a auf output
    // mcp_writeRegister(0x01, 0x00);  // bank b auf output

    for(int i = 0; i<config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i,1);
    }

    // Autostart Einträge aktivieren für 12V
    for(int f=0; f<config.mcp.numberOfRelaisActive; f++){
        if( strcmp(deviceActiveOnStart[f],"true") == 0){
            mcp_digitalWrite(f, 0);
            delay(333);
        }
    }
        

    // Wenn es zwischen 21:00 Uhr Abends und 05:59 Uhr Morgens ist, auch das Licht(Relais 16) anschalten      
    if( (ts->tm_hour > 20 && ts->tm_hour < 24) || (ts->tm_hour >= 0 && ts->tm_hour < 6) ) {
        delay(333);
        mcp_digitalWrite(15, 0); 
    }










    if (ini_parse("/Energiebox/230V/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/230V/config.ini'\n");
        return 1;
    }
    
    // 230 Volt
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) {
        printf("wiringPiI2CSetup error!!!");
        return -1;
    }
    mcp_initReg();

    // Alle als OUTPUT definieren und ausschalten
    for(int i = 0; i<config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i,1);
    }

    // Autostart Einträge aktivieren für 230V
    //for(int f=0; f<config.mcp.numberOfRelaisActive; f++){
    //    if( strcmp(deviceActiveOnStart[f],"true") == 0){
    //        mcp_digitalWrite(f, 0);
    //        delay(333);
    //    }
    //}
    return 0;
}
