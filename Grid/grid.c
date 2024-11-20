#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mymcp23017.h"
#include <unistd.h>


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





int main(int argc, char**argv) {
    if(wiringPiSetup()<0) {
        printf("wiringPiSetup error!!!");
        return -1;
    }

    mcp_begin(2);
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
        sleep(1);
    }
    
    setBit(0, 1); // Relais einschalten 

    setBit(1, 0); // Relais einschalten 
    

    return 0;
}
