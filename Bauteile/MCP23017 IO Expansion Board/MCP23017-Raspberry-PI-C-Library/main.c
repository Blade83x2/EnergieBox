#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mymcp23017.h"

int main(int argc, char**argv)
{
    if(wiringPiSetup()<0)
    {
        printf("wiringPiSetup error!!!");
        return -1;
    }

    //XXX X  A2 A1 A0
    //010 0  1  1  1      0x27       7
    //010 0  1  1  0      0x26       6
    //010 0  1  0  1      0x25       5
    //010 0  1  0  0      0x24       4
    //010 0  0  1  1      0x23       3
    //010 0  0  1  0      0x22       2
    //010 0  0  0  1      0x21       1
    //010 0  0  0  0      0x20       0

    // 12 Volt
    mcp_begin(2);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0)
    {
        printf("wiringPiI2CSetup error!!!");
        return -1;
    }
    mcp_initReg();

    // Alle als OUTPUT definieren und ausschalten
    for(int i = 0; i<16; i++)
    {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i,1);
    }


    // Relais 1 und 2 einschalten
    mcp_digitalWrite(0, 0);
    mcp_digitalWrite(1, 0);

    // 230 Volt
    mcp_begin(7);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0)  
    {
        printf("wiringPiI2CSetup error!!!");
        return -1;
    }
    mcp_initReg();

    // Alle als OUTPUT definieren und ausschalten
    for(int i = 0; i<16; i++)
    {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i,1);
    }





    return 0;
}
