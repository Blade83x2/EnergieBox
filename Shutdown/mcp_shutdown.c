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

char command[100];

static int handlerGrid(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("mcp", "address")) {  pconfig->mcp.address = atoi(value); } 
    else if(MATCH("mcp", "numberOfRelaisActive")) { pconfig->mcp.numberOfRelaisActive = atoi(value); } 
    else { return 0; }
    return 1;
}







int main(int argc, char**argv) {
    if(wiringPiSetup()<0) {
        printf("wiringPiSetup error!!!");
        return -1;
    }
    //XXX X  A2 A1 A0
    //010 0  1  1  1      0x27       7   230Volt Bank
    //010 0  0  1  0      0x26       6   12Volt Bank
    //010 0  0  1  0      0x22       2   Grid Bank
    // 12 Volt
    mcp_begin(6);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) {
        printf("wiringPiI2CSetup error!!!");
        return -1;
    }
    mcp_initReg();
    //  Alle als OUTPUT definieren und ausschalten
    for(int i = 0; i<16; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i,1);
        sprintf(command, "sh /Energiebox/12V/setIni.sh %d %d", (i+1), 0);
        system(command);
        sleep(0.1);
    }


    // 230 Volt
    mcp_begin(7);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) {
        printf("wiringPiI2CSetup error!!!");
        return -1;
    }
    mcp_initReg();

    // Alle als OUTPUT definieren und ausschalten
    for(int i = 0; i<16; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i,1);
        sprintf(command, "sh /Energiebox/230V/setIni.sh %d %d", (i+1), 0);
        system(command);
        sleep(0.1);
    }
    

    ////////////////////
    ///// GRID      ////
    ////////////////////
    // config Objekt überladen
    if (ini_parse("/Energiebox/Grid/config.ini", handlerGrid, &config) < 0) {
        printf("Can't load '/Energiebox/Grid/config.ini'\n");
        return 1;
    }
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) {
        printf("wiringPi I2C Setup error!!!");
        return -1;
    }
    mcp_initReg();
    // Alle als OUTPUT definieren und ausschalten
    for(int i = 0; i<config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i, 1);
        sleep(0.1);
    }


    return 0;
}
