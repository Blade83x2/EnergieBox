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
#include <stdlib.h>  // atoi()

// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
    int maxOutputPower;
    int maxPConverter;
    int maxPMicroController;
} mcp_setup;

// Relais Strukturen
typedef struct {
    const char *name;
    bool activateOnStart;
    int eltakoState;
    int pMax;
    const char *autoStart;
    const char *autoStop;
} relais_config;

// xV Setup
typedef struct {
    const char *traceTxtFilePath;
} system_setup;

// Konfigurations Struktur
typedef struct {
    mcp_setup mcp;
    relais_config r[16];
    system_setup system;
} configuration;

char command[100];
char deviceNames[16][40];
char deviceActiveOnStart[16][6];
int deviceEltakoState[16][1];
int devicePMax[16];
int pMaxCurrent;

static int handler(void *config, const char *section, const char *name, const char *value) {
    configuration *pconfig = (configuration *)config;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("system", "traceTxtFilePath")) {
        pconfig->system.traceTxtFilePath = strdup(value);
    } else if (MATCH("mcp", "address")) {
        pconfig->mcp.address = atoi(value);
    } else if (MATCH("mcp", "numberOfRelaisActive")) {
        pconfig->mcp.numberOfRelaisActive = atoi(value);
    } else if (MATCH("mcp", "maxPConverter")) {
        pconfig->mcp.maxPConverter = atoi(value);
    } else if (MATCH("mcp", "maxOutputPower")) {
        pconfig->mcp.maxOutputPower = atoi(value);
    } else if (MATCH("mcp", "maxPMicroController")) {
        pconfig->mcp.maxPMicroController = atoi(value);
    } else {
        int relaisNum = 0;
        if (sscanf(section, "Relais %d", &relaisNum) == 1) {
            if (relaisNum >= 1 && relaisNum <= 16) {
                int idx = relaisNum - 1;
                if (strcmp(name, "name") == 0) {
                    strcpy(deviceNames[idx], strdup(value));
                } else if (strcmp(name, "activateOnStart") == 0) {
                    strcpy(deviceActiveOnStart[idx], value);
                } else if (strcmp(name, "eltakoState") == 0) {
                    pconfig->r[idx].eltakoState = atoi(value);
                } else if (strcmp(name, "pMax") == 0) {
                    pconfig->r[idx].pMax = atoi(value);
                } else {
                    return 0;
                }
                return 1;
            }
        }
        return 0;
    }
    return 1;
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
    if (Get_Port & (1 << PIN)) {
        return 0;
    } else {
        return 1;
    }
}

int main(int argc, char **argv) {
    configuration config;
    /////////////////////
    //// 12 Volt  ///////
    /////////////////////
    // Konfiguration von 12V in config laden
    if (ini_parse("/Energiebox/12V/config.ini", handler, &config) < 0) {
        fprintf(stderr, "Can't load '/Energiebox/12V/config.ini\n");
        exit(EXIT_FAILURE);
    }
    if (wiringPiSetup() < 0) {
        fprintf(stderr, "wiringPiSetup error\n");
        exit(EXIT_FAILURE);
    }
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if (fd < 0) {
        fprintf(stderr, "wiringPi I2C Setup error\n");
        exit(EXIT_FAILURE);
    }
    mcp_initReg();  // Register Bits
    // Alle als Output definieren und auf aus stellen
    for (int i = 0; i < config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i, 1);
        sprintf(command, "bash /Energiebox/12V/setIni.sh %d %d", (i + 1), 0);
        system(command);
        sleep(0.2);
    }
    pMaxCurrent = 0;  // verfügbare Watt ausrechnen
    // Verbrauch von MicroController mit einbezoehen
    pMaxCurrent += config.mcp.maxPMicroController;
    // derzeitige Leistung aller eingeschalteten Geräte addieren und in pMaxCurrent speichern
    for (int w = 0; w < config.mcp.numberOfRelaisActive; w++) {
        if (getBit(w + 1) == 1) {
            pMaxCurrent += devicePMax[w];
        }
        sleep(0.1);
    }
    // Jedes Relais durchlaufen
    for (int f = 0; f < config.mcp.numberOfRelaisActive; f++) {
        // wenn autostart aktiviert ist,
        if (strcmp(deviceActiveOnStart[f], "true") == 0) {
            // Wenn zusätzliche Leistung + aktueller Leistungsverbrauch kleiner oder gleich maximal
            // Abgabe Leistung vom DC-DC Konverter in Watt
            if ((pMaxCurrent + devicePMax[f]) <= config.mcp.maxPConverter) {
                // Relais einschalten
                mcp_digitalWrite(f, 0);
                // eltakostatus in config schreiben
                sprintf(command, "bash /Energiebox/12V/setIni.sh %d %d", (f + 1), 1);
                system(command);
                pMaxCurrent += devicePMax[f];
                // printf("Neuer Verbrauch aller Geräte derzeit: %d Watt\n", pMaxCurrent);
                sleep(0.8);
            } else {
                // Gerät kann nicht eingeschaltet werden weil nicht genug Leistung vorhanden ist
                printf("\e[0;31m%s benötigt %d Watt. Derzeit ist nicht genügend Leistung verfügbar!\n", deviceNames[f], devicePMax[f]);
            }
        }
    }
    ////////////////////
    ///// 230 Volt  ////
    ////////////////////
    // config Objekt überladen
    if (ini_parse("/Energiebox/230V/config.ini", handler, &config) < 0) {
        fprintf(stderr, "Can't load '/Energiebox/230V/config.ini\n");
        exit(EXIT_FAILURE);
    }
    // Wirig PI prüfen
    if (wiringPiSetup() < 0) {
        fprintf(stderr, "wiringPiSetup error\n");
        exit(EXIT_FAILURE);
    }
    // MCP  Portexpander
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if (fd < 0) {
        fprintf(stderr, "wiringPi I2C Setup error\n");
        exit(EXIT_FAILURE);
    }
    mcp_initReg();
    // Alle als OUTPUT definieren und ausschalten
    for (int i = 0; i < config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i, 1);
        sprintf(command, "bash /Energiebox/230V/setIni.sh %d %d", (i + 1), 0);
        system(command);
        sleep(0.2);
    }
    // Autostart Einträge aktivieren für 230V

    for (int f = 0; f < config.mcp.numberOfRelaisActive; f++) {
        // wenn autostart aktiviert ist,
        if (strcmp(deviceActiveOnStart[f], "true") == 0) {
            // verfügbare Watt ausrechnen
            pMaxCurrent = 0;
            // derzeitige Leistung aller eingeschalteten Geräte addieren und in pMaxCurrent
            // speichern
            for (int w = 0; w < config.mcp.numberOfRelaisActive; w++) {
                if (getBit(w + 1) == 1) {
                    pMaxCurrent += devicePMax[w];
                }
            }
            // Wenn zusätzliche Leistung + aktuelle Leistung kleiner oder gleich maximal Abgabe
            // Leistung in Watt
            if ((pMaxCurrent + devicePMax[f]) <= config.mcp.maxOutputPower) {
                // Relais einschalten
                mcp_digitalWrite(f, 0);
                // eltakostatus in config schreiben
                sprintf(command, "bash /Energiebox/230V/setIni.sh %d %d", (f + 1), 1);
                system(command);
                sleep(0.8);
            } else {
                printf("\e[0;31m%s benötigt %d Watt. Derzeit maximal verfügbar: %d Watt!\n", deviceNames[f], devicePMax[f], config.mcp.maxOutputPower - pMaxCurrent);
            }
        }
    }

    ////////////////////
    ///// GRID      ////
    ////////////////////
    // config Objekt überladen
    if (ini_parse("/Energiebox/Grid/config.ini", handler, &config) < 0) {
        fprintf(stderr, "Can't load '/Energiebox/Grid/config.ini\n");
        exit(EXIT_FAILURE);
    }
    // Wirig PI prüfen
    if (wiringPiSetup() < 0) {
        fprintf(stderr, "wiringPiSetup error\n");
        exit(EXIT_FAILURE);
    }
    // MCP  Portexpander
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if (fd < 0) {
        fprintf(stderr, "wiringPi I2C Setup error\n");
        exit(EXIT_FAILURE);
    }
    mcp_initReg();
    // Alle als OUTPUT definieren und ausschalten
    for (int i = 0; i < config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i, 1);
        sleep(0.1);
    }
    sprintf(command, "rm -f /Energiebox/Grid/isLoading.lock");
    system(command);
    return 0;
}
