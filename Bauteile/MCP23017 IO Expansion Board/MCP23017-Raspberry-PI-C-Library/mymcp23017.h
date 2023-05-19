#ifndef _MY_MCP23017_H_
#define _MY_MCP23017_H_

#include <wiringPi.h>
#include <stdint.h>


extern uint8_t i2caddr;
extern int fd;

void mcp_begin(uint8_t addr);
void mcp_initReg(void);

void mcp_pinMode(uint8_t p, uint8_t d);
void mcp_digitalWrite(uint8_t p, uint8_t d);
void mcp_pullUp(uint8_t p, uint8_t d);
uint8_t mcp_digitalRead(uint8_t p);

void mcp_writeGPIOAB(uint16_t);
uint16_t mcp_readGPIOAB(void);
uint8_t mcp_readGPIO(uint8_t b);

void mcp_setupInterrupts(uint8_t mirroring, uint8_t open, uint8_t polarity);
void mcp_setupInterruptPin(uint8_t p, uint8_t mode);
uint8_t mcp_getLastInterruptPin(void);
uint8_t mcp_getLastInterruptPinValue(void);




uint8_t mcp_bitForPin(uint8_t pin);
uint8_t mcp_regForPin(uint8_t pin, uint8_t portAaddr, uint8_t portBaddr);

uint8_t mcp_readRegister(uint8_t addr);
void mcp_writeRegister(uint8_t addr, uint8_t value);

void mcp_updateRegisterBit(uint8_t p, uint8_t pValue, uint8_t portAaddr, uint8_t portBaddr);


#define MCP23017_ADDRESS 0x20

// registers
#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA 0x06
#define MCP23017_INTCONA 0x08
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14


#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15

#define MCP23017_INT_ERR 255

#endif
