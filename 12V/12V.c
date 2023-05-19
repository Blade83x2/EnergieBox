#include <stdio.h>
#include <stdlib.h> // atoi()
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mymcp23017.h"
#include <unistd.h> // sleep()
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int getBit(int Port);
void setBit(int Port, int Status);
bool checkMainParameter(char* paramName, int number, void* config);
int showHelp(char**argv, void* config);

// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
    int maxPConverter;
    int maxPMicroController;
} mcp_setup;

// Relais Struktur
typedef struct {
    const char* name;
    bool activateOnStart;
    int eltakoState;
    int pMax;
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
int deviceEltakoState[16][1];
int devicePMax[16];
int pMaxCurrent;
int devicePowerMax[16][4];
char command[100];

static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("mcp", "address")) {  pconfig->mcp.address = atoi(value); }
    else if(MATCH("mcp", "numberOfRelaisActive")) { pconfig->mcp.numberOfRelaisActive = atoi(value); }
    else if(MATCH("mcp", "maxPConverter")) { pconfig->mcp.maxPConverter = atoi(value); }
    else if(MATCH("mcp", "maxPMicroController")) { pconfig->mcp.maxPMicroController = atoi(value); }

    else if(MATCH("Relais 1", "name")) { strcpy(deviceNames[0], strdup(value)); }
    else if(MATCH("Relais 1", "activateOnStart")) { strcpy(deviceActiveOnStart[0], value); }
    else if(MATCH("Relais 1", "eltakoState")) { pconfig->r1.eltakoState = atoi(value); }
    else if(MATCH("Relais 1", "pMax")) { pconfig->r1.pMax = atoi(value); }

    else if(MATCH("Relais 2", "name")) { strcpy(deviceNames[1], strdup(value)); }
    else if(MATCH("Relais 2", "activateOnStart")) { strcpy(deviceActiveOnStart[1], value); }
    else if(MATCH("Relais 2", "eltakoState")) { pconfig->r2.eltakoState = atoi(value); }
    else if(MATCH("Relais 2", "pMax")) { pconfig->r2.pMax = atoi(value); }

    else if(MATCH("Relais 3", "name")) { strcpy(deviceNames[2], strdup(value)); }
    else if(MATCH("Relais 3", "activateOnStart")) { strcpy(deviceActiveOnStart[2], value); }
    else if(MATCH("Relais 3", "eltakoState")) { pconfig->r3.eltakoState = atoi(value); }
    else if(MATCH("Relais 3", "pMax")) { pconfig->r3.pMax = atoi(value); }

    else if(MATCH("Relais 4", "name")) { strcpy(deviceNames[3], strdup(value)); } 
    else if(MATCH("Relais 4", "activateOnStart")) {  strcpy(deviceActiveOnStart[3], value);  } 
    else if(MATCH("Relais 4", "eltakoState")) { pconfig->r4.eltakoState = atoi(value); }
    else if(MATCH("Relais 4", "pMax")) { pconfig->r4.pMax = atoi(value); }

    else if(MATCH("Relais 5", "name")) { strcpy(deviceNames[4], strdup(value)); } 
    else if(MATCH("Relais 5", "activateOnStart")) { strcpy(deviceActiveOnStart[4], value);  } 
    else if(MATCH("Relais 5", "eltakoState")) { pconfig->r5.eltakoState = atoi(value); }
    else if(MATCH("Relais 5", "pMax")) { pconfig->r5.pMax = atoi(value); }

    else if(MATCH("Relais 6", "name")) { strcpy(deviceNames[5], strdup(value)); } 
    else if(MATCH("Relais 6", "activateOnStart")) { strcpy(deviceActiveOnStart[5], value);  } 
    else if(MATCH("Relais 6", "eltakoState")) { pconfig->r6.eltakoState = atoi(value); }
    else if(MATCH("Relais 6", "pMax")) { pconfig->r6.pMax = atoi(value); }

    else if(MATCH("Relais 7", "name")) { strcpy(deviceNames[6], strdup(value)); } 
    else if(MATCH("Relais 7", "activateOnStart")) { strcpy(deviceActiveOnStart[6], value); } 
    else if(MATCH("Relais 7", "eltakoState")) { pconfig->r7.eltakoState = atoi(value); }
    else if(MATCH("Relais 7", "pMax")) { pconfig->r7.pMax = atoi(value); }

    else if(MATCH("Relais 8", "name")) { strcpy(deviceNames[7], strdup(value)); } 
    else if(MATCH("Relais 8", "activateOnStart")) { strcpy(deviceActiveOnStart[7], value);  } 
    else if(MATCH("Relais 8", "eltakoState")) { pconfig->r8.eltakoState = atoi(value); }
    else if(MATCH("Relais 8", "pMax")) { pconfig->r8.pMax = atoi(value); }

    else if(MATCH("Relais 9", "name")) { strcpy(deviceNames[8], strdup(value)); } 
    else if(MATCH("Relais 9", "activateOnStart")) { strcpy(deviceActiveOnStart[8], value);  } 
    else if(MATCH("Relais 9", "eltakoState")) { pconfig->r9.eltakoState = atoi(value); }
    else if(MATCH("Relais 9", "pMax")) { pconfig->r9.pMax = atoi(value); }

    else if(MATCH("Relais 10", "name")) { strcpy(deviceNames[9], strdup(value)); } 
    else if(MATCH("Relais 10", "activateOnStart")) {  strcpy(deviceActiveOnStart[9], value);  } 
    else if(MATCH("Relais 10", "eltakoState")) { pconfig->r10.eltakoState = atoi(value); }
    else if(MATCH("Relais 10", "pMax")) { pconfig->r10.pMax = atoi(value); }

    else if(MATCH("Relais 11", "name")) { strcpy(deviceNames[10], strdup(value)); } 
    else if(MATCH("Relais 11", "activateOnStart")) {  strcpy(deviceActiveOnStart[10], value);  } 
    else if(MATCH("Relais 11", "eltakoState")) { pconfig->r11.eltakoState = atoi(value); }
    else if(MATCH("Relais 11", "pMax")) { pconfig->r11.pMax = atoi(value); }

    else if(MATCH("Relais 12", "name")) { strcpy(deviceNames[11], strdup(value)); } 
    else if(MATCH("Relais 12", "activateOnStart")) {  strcpy(deviceActiveOnStart[11], value);  } 
    else if(MATCH("Relais 12", "eltakoState")) { pconfig->r12.eltakoState = atoi(value); }
    else if(MATCH("Relais 12", "pMax")) { pconfig->r12.pMax = atoi(value); }

    else if(MATCH("Relais 13", "name")) { strcpy(deviceNames[12], strdup(value)); } 
    else if(MATCH("Relais 13", "activateOnStart")) {  strcpy(deviceActiveOnStart[12], value);  } 
    else if(MATCH("Relais 13", "eltakoState")) { pconfig->r13.eltakoState = atoi(value); }
    else if(MATCH("Relais 13", "pMax")) { pconfig->r13.pMax = atoi(value); }

    else if(MATCH("Relais 14", "name")) { strcpy(deviceNames[13], strdup(value)); } 
    else if(MATCH("Relais 14", "activateOnStart")) {  strcpy(deviceActiveOnStart[13], value);  } 
    else if(MATCH("Relais 14", "eltakoState")) { pconfig->r14.eltakoState = atoi(value); }
    else if(MATCH("Relais 14", "pMax")) { pconfig->r14.pMax = atoi(value); }

    else if(MATCH("Relais 15", "name")) { strcpy(deviceNames[14], strdup(value)); } 
    else if(MATCH("Relais 15", "activateOnStart")) {  strcpy(deviceActiveOnStart[14], value);  } 
    else if(MATCH("Relais 15", "eltakoState")) { pconfig->r15.eltakoState = atoi(value); }
    else if(MATCH("Relais 15", "pMax")) { pconfig->r15.pMax = atoi(value); }

    else if(MATCH("Relais 16", "name")) { strcpy(deviceNames[15], strdup(value)); } 
    else if(MATCH("Relais 16", "activateOnStart")) {  strcpy(deviceActiveOnStart[15], value);  } 
    else if(MATCH("Relais 16", "eltakoState")) { pconfig->r16.eltakoState = atoi(value); }
    else if(MATCH("Relais 16", "pMax")) { pconfig->r16.pMax = atoi(value); }
    else { return 0; }
    return 1;
}

// Gibt gespeicherten Zustandswert von einem Relais zurück
int getElkoState(int relais, void* config){
    configuration* pconfig = (configuration*)config;
    switch (relais) {
        case 1:
            return pconfig->r1.eltakoState;
            break;
        case 2:
            return pconfig->r2.eltakoState;
            break;
        case 3:
            return pconfig->r3.eltakoState;
            break;
        case 4:
            return pconfig->r4.eltakoState;
            break;
        case 5:
            return pconfig->r5.eltakoState;
            break;
        case 6:
            return pconfig->r6.eltakoState;
            break;
        case 7:
            return pconfig->r7.eltakoState;
            break;
        case 8:
            return pconfig->r8.eltakoState;
            break;
        case 9:
            return pconfig->r9.eltakoState;
            break;
        case 10:
            return pconfig->r10.eltakoState;
            break;
        case 11:
            return pconfig->r11.eltakoState;
            break;
        case 12:
            return pconfig->r12.eltakoState;
            break;
        case 13:
            return pconfig->r13.eltakoState;
            break;
        case 14:
            return pconfig->r14.eltakoState;
            break;
        case 15:
            return pconfig->r15.eltakoState;
            break;
        case 16:
            return pconfig->r16.eltakoState;
            break;
    }
    return 0;
}

// Gibt gesamten Watt Verbrauch zurück
int getCurrentPower(void * config) {
     configuration* pconfig = (configuration*)config;
     int watt;
     watt=0;
     watt += pconfig->mcp.maxPMicroController;
     // checken welche geräte an sind, Verbrauch addieren
     if(pconfig->r1.eltakoState==1) watt += pconfig->r1.pMax;
     if(pconfig->r2.eltakoState==1) watt += pconfig->r2.pMax;
     if(pconfig->r3.eltakoState==1) watt += pconfig->r3.pMax;
     if(pconfig->r4.eltakoState==1) watt += pconfig->r4.pMax;
     if(pconfig->r5.eltakoState==1) watt += pconfig->r5.pMax;
     if(pconfig->r6.eltakoState==1) watt += pconfig->r6.pMax;
     if(pconfig->r7.eltakoState==1) watt += pconfig->r7.pMax;
     if(pconfig->r8.eltakoState==1) watt += pconfig->r8.pMax;
     if(pconfig->r9.eltakoState==1) watt += pconfig->r9.pMax;
     if(pconfig->r10.eltakoState==1) watt += pconfig->r10.pMax;
     if(pconfig->r11.eltakoState==1) watt += pconfig->r11.pMax;
     if(pconfig->r12.eltakoState==1) watt += pconfig->r12.pMax;
     if(pconfig->r13.eltakoState==1) watt += pconfig->r13.pMax;
     if(pconfig->r14.eltakoState==1) watt += pconfig->r14.pMax;
     if(pconfig->r15.eltakoState==1) watt += pconfig->r15.pMax;
     if(pconfig->r16.eltakoState==1) watt += pconfig->r16.pMax;
     return watt;
}

// Gibt benötigte Watt vom einem Gerät zurück
int getDevicePower(int relais, void * config) {
     configuration* pconfig = (configuration*)config;
     if(relais==1) return pconfig->r1.pMax;
     if(relais==2) return  pconfig->r2.pMax;
     if(relais==3) return  pconfig->r3.pMax;
     if(relais==4) return  pconfig->r4.pMax;
     if(relais==5) return  pconfig->r5.pMax;
     if(relais==6) return  pconfig->r6.pMax;
     if(relais==7) return  pconfig->r7.pMax;
     if(relais==8) return  pconfig->r8.pMax;
     if(relais==9) return  pconfig->r9.pMax;
     if(relais==10) return  pconfig->r10.pMax;
     if(relais==11) return  pconfig->r11.pMax;
     if(relais==12) return  pconfig->r12.pMax;
     if(relais==13) return  pconfig->r13.pMax;
     if(relais==14) return  pconfig->r14.pMax;
     if(relais==15) return  pconfig->r15.pMax;
     if(relais==16) return  pconfig->r16.pMax;
     return 0;
}

// Gibt verfügbaren Watt Restwert zurück
int getRestPower(void * config) {
     configuration* pconfig = (configuration*)config;
     int watt;
     watt=0;
     watt += pconfig->mcp.maxPMicroController;
     // checken welche geräte an sind, watt addieren
     if(pconfig->r1.eltakoState==1) watt += pconfig->r1.pMax;
     if(pconfig->r2.eltakoState==1) watt += pconfig->r2.pMax;
     if(pconfig->r3.eltakoState==1) watt += pconfig->r3.pMax;
     if(pconfig->r4.eltakoState==1) watt += pconfig->r4.pMax;
     if(pconfig->r5.eltakoState==1) watt += pconfig->r5.pMax;
     if(pconfig->r6.eltakoState==1) watt += pconfig->r6.pMax;
     if(pconfig->r7.eltakoState==1) watt += pconfig->r7.pMax;
     if(pconfig->r8.eltakoState==1) watt += pconfig->r8.pMax;
     if(pconfig->r9.eltakoState==1) watt += pconfig->r9.pMax;
     if(pconfig->r10.eltakoState==1) watt += pconfig->r10.pMax;
     if(pconfig->r11.eltakoState==1) watt += pconfig->r11.pMax;
     if(pconfig->r12.eltakoState==1) watt += pconfig->r12.pMax;
     if(pconfig->r13.eltakoState==1) watt += pconfig->r13.pMax;
     if(pconfig->r14.eltakoState==1) watt += pconfig->r14.pMax;
     if(pconfig->r15.eltakoState==1) watt += pconfig->r15.pMax;
     if(pconfig->r16.eltakoState==1) watt += pconfig->r16.pMax;
     return pconfig->mcp.maxPConverter - watt;
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
        printf("\n\e[0;30m\e[47m 12V\tStatus\t> %dW\t Gerätename   \e[0m\n", getCurrentPower(&config));
        for(int x=1; x<=config.mcp.numberOfRelaisActive; x++) {
              printf("\e[0;36m %d\t%s %s\t %d%s \t%s  \e[0m\n", x, ((getElkoState(x, &config)==0)?"\e[0;31m":"\e[0;32m"), ((getElkoState(x, &config)==0)?"aus":"an"), 
              (getDevicePower(x, &config)), "W", deviceNames[x-1] );
        }
        printf("\n");
    }
    else if(argc == 2) {
        if(!checkMainParameter("relaisNumber", atoi(argv[1]), &config)) {
            return showHelp(argv, &config);
        }
        // Relais Nr. übergeben und Elkostate auf console printen
        printf("%d",  getElkoState(atoi(argv[1]), &config));
    }
    else if(argc == 3) {
        if(!checkMainParameter("relaisNumber", atoi(argv[1]), &config) || !checkMainParameter("relaisZustand", atoi(argv[2]), &config)) {
            return showHelp(argv, &config);
        }
        // wenn gewünschter relaiszustand und config stand gleich sind, nix machen
        if(atoi(argv[2]) != getElkoState(atoi(argv[1]), &config)){ 
            // wenn eingeschaltet wird
            if(atoi(argv[2])==1) { 
                    // prüfen ob DC Konverter genug Leistung hat
                    if(getRestPower(&config) >= getDevicePower(atoi(argv[1]), &config) && getDevicePower(atoi(argv[1]), &config) <= config.mcp.maxPConverter) {
                         // impuls für Elko 
                         setBit(atoi(argv[1])-1, atoi(argv[2])==1?0:1); // Relais einschalten 
                        //  elkoState in config.ini schreiben
                         sprintf(command, "sudo sh /Energiebox/12V/setIni.sh %d %d", atoi(argv[1]), atoi(argv[2]));
                         system(command);
                         sleep(0.6);
                         // Kommentar ausklammern wenn eltakos für 12V verfügbar sind
                         //setBit(atoi(argv[1])-1, 1); // Relais ausschalten (1 setzt bit auf 0, 0 setzt bit auf 1)
                    }
                     else {
                        // Nicht genug Watt verfügbar für neues Gerät
                        printf("\e[0;31mDas Gerät benötigt %d Watt aber es sind nur %d Watt verfügbar! Andere Geräte ausschalten..!?\n", 
                        getDevicePower(atoi(argv[1]), &config), getRestPower(&config));
                        return 1;
                    }
              }
                else {
                        // wenn ausgeschaltet wird
                         // impuls für Elko 
                         // Kommentar ausklammern wenn 12V mit Elkos laufen
                         //setBit(atoi(argv[1])-1, 0); // Relais einschalten 
                         //  elkoState in config.ini schreiben
                         sprintf(command, "sudo sh /Energiebox/12V/setIni.sh %d %d", atoi(argv[1]), atoi(argv[2]));
                         system(command);
                         sleep(0.6);
                         // Kommentar ausklammern wenn 12V mit Elkos laufen
                         //setBit(atoi(argv[1])-1, 1); // Relais ausschalten (1 setzt bit auf 0, 0 setzt bit auf 1)
                         setBit(atoi(argv[1])-1, atoi(argv[2])==1?0:1); // Relais ausschalten
              }
        }
    }
    else if(argc == 4) {
        if(!checkMainParameter("relaisNumber", atoi(argv[1]), &config) || !checkMainParameter("relaisZustand", atoi(argv[2]), &config) || !checkMainParameter("relaisTime", atoi(argv[3]), &config )) {
            return showHelp(argv, &config);
        }
       // wenn gewünschter relaiszustand und config stand gleich sind, nix machen
        if(atoi(argv[2]) != getElkoState(atoi(argv[1]), &config)){ 
            sleep(atoi(argv[3]));
            // wenn eingeschaltet wird
            if(atoi(argv[2])==1) { 
                    // prüfen ob DC Konverter  genug Leistung hat
                    if(getRestPower(&config) >= getDevicePower(atoi(argv[1]), &config) && getDevicePower(atoi(argv[1]), &config) <= config.mcp.maxPConverter) {
                         // impuls für Elko 
                         setBit(atoi(argv[1])-1, atoi(argv[2])==1?0:1); // Relais einschalten 
                         //  elkoState in config.ini schreiben
                         sprintf(command, "sudo sh /Energiebox/12V/setIni.sh %d %d", atoi(argv[1]), atoi(argv[2]));
                         system(command);
                         sleep(0.6);
                         // Wenn 12V mit Eltakos läuft, folgenden Kommentar entfernen
                         //setBit(atoi(argv[1])-1, 1); // Relais ausschalten (1 setzt bit auf 0, 0 setzt bit auf 1)
                    }
                     else {
                        // Nicht genug Watt verfügbar für neues Gerät
                        printf("\e[0;31mDas Gerät benötigt %d Watt aber es sind nur %d Watt verfügbar! Andere Geräte ausschalten..!?\n", 
                        getDevicePower(atoi(argv[1]), &config), getRestPower(&config));
                        return 1;
                    }
                }
                else {
                        // wenn ausgeschaltet wird
                         // impuls für Elko
                         // Wenn 12V mit Eltakos läuft, folgende Zeile wieder einbinden 
                         //setBit(atoi(argv[1])-1, 0); // Relais einschalten 
                         //  elkoState in config.ini schreiben
                         sprintf(command, "sudo sh /Energiebox/12V/setIni.sh %d %d", atoi(argv[1]), atoi(argv[2]));
                         system(command);
                         sleep(0.6);
                         setBit(atoi(argv[1])-1, 1); // Relais ausschalten (1 setzt bit auf 0, 0 setzt bit auf 1)
                }
        }
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
        if ( number < 1 || number > 84600 )
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
    printf("  In der Konfigurationsdatei können Namen, Verbrauch & Autostart für angeschlossene Geräte vergeben werden!\n");
    printf("  sudo nano /Energiebox/%s/config.ini\e[0m \n\n", argv[0]);
    return -1;
}
