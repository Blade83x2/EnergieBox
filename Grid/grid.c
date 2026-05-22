/*
 * Controller & Management Software for Battery charging
 * Vendor: Johannes Krämer
 * Version: 1.0
 * Date: 20.11.2024
 */
#include <stdio.h>
#include <stdlib.h>  // atoi()
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mymcp23017.h"
#include "iniparse.h"
#include <unistd.h>  // sleep()
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
} mcp_setup;

// GRID Setup
typedef struct {
    float supplyMaxCurrent;
    float supplyMaxVoltage;
    int supplyMinLoadWh;
    int supplyMaxLoadWh;
    float battVoltageStartLoading;
    int loadingCapacityWh;
} grid_setup;

// Strukt
typedef struct {
    const char* mysqlCfgPath;
    const char* lockFilePath;
    const char* PIDFilePath;
    const char* readallCmd;
} system_setup;

// config
typedef struct {
    mcp_setup mcp;
    grid_setup grid;
    system_setup system;
} configuration;

// System Kommandos String
char command[100];

// Netzteilparameter in config.txt eintragen!
static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("mcp", "address")) {
        pconfig->mcp.address = atoi(value);
    } else if (MATCH("mcp", "numberOfRelaisActive")) {
        pconfig->mcp.numberOfRelaisActive = atoi(value);
    } else if (MATCH("grid", "supplyMaxCurrent")) {
        pconfig->grid.supplyMaxCurrent = atof(value);
    } else if (MATCH("system", "lockFilePath")) {
        pconfig->system.lockFilePath = strdup(value);
    } else if (MATCH("system", "PIDFilePath")) {
        pconfig->system.PIDFilePath = strdup(value);
    } else if (MATCH("system", "readallCmd")) {
        pconfig->system.readallCmd = strdup(value);

    } else if (MATCH("grid", "supplyMinLoadWh")) {
        pconfig->grid.supplyMinLoadWh = atoi(value);
    } else if (MATCH("grid", "supplyMaxLoadWh")) {
        pconfig->grid.supplyMaxLoadWh = atoi(value);
    } else if (MATCH("grid", "battVoltageStartLoading")) {
        pconfig->grid.battVoltageStartLoading = atof(value);
    } else if (MATCH("grid", "loadingCapacityWh")) {
        pconfig->grid.loadingCapacityWh = atoi(value);
    } else {
        return 0;
    }
    return 1;
}

// ==========================================================
// Liest Batteriespannung aus Python Script
// ==========================================================
float getBatteryVoltage(const char* script) {
    FILE* fp;
    char buffer[512];
    float voltage = 0.0f;
    // Python Script starten
    fp = popen(script, "r");
    if (fp == NULL) {
        perror("popen");
        return 0.0f;
    }
    // Zeilenweise lesen
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Gewünschte Zeile finden
        if (strstr(buffer, "Batterie: Aktuelle Spannung in Volt =")) {
            // Spannung auslesen
            sscanf(buffer, "Batterie: Aktuelle Spannung in Volt = %fV", &voltage);
            break;
        }
    }
    pclose(fp);
    return voltage;
}

// Schreibt Bit für Relaiszustand
void setBit(int Port, int Status) {
    if (Port < 0 || Port > 15) return;
    uint8_t reg = (Port < 8) ? 0x12 : 0x13;
    int PIN = (Port < 8) ? Port : (Port % 8);
    int Get_Port = mcp_readRegister(reg);
    Get_Port = (Status == 1) ? (Get_Port | (1 << PIN)) : (Get_Port & ~(1 << PIN));
    mcp_writeRegister((Port > -1 && Port < 8) ? 0x12 : 0x13, Get_Port);
}

// Zeigt Hilfe auf Console an
int showHelp(char** argv, void* config) {
    printf("\n  %s -w <Wh>\t\tLädt <Wh> Wattstunden aus dem Netz\n", argv[0]);
    printf("  %s -s \t\tBeendet den Ladevorgang\n", argv[0]);
    printf("  %s -h\t\tZeigt diese Hilfe an\n", argv[0]);
    printf("\n Beispiel:\n");
    printf("  %s -w 200\t\tLädt 200 Wh in die Batterie\n", argv[0]);
    printf("  %s -s \t\tStoppt Ladevorgang\n\n", argv[0]);
    return -1;
}

// Prüft ob str Numerisch ist
bool isNumeric(const char* str) {
    while (*str != '\0') {
        if (*str < '0' || *str > '9') return false;
        str++;
    }
    return true;
}

// Gibt Zeitformat h:m:s zurück
void formatSecondsToHMS(int seconds, char* buffer, size_t size) {
    int h = seconds / 3600;
    int m = (seconds % 3600) / 60;
    int s = seconds % 60;
    snprintf(buffer, size, "%02d:%02d:%02d", h, m, s);
}

// Programmstart
int main(int argc, char* argv[]) {
    configuration config;
    if (ini_parse("/Energiebox/Grid/config.ini", handler, &config) < 0) {
        fprintf(stderr, "Can't load '/Energiebox/Grid/config.ini\n");
        return 1;
    }
    if (wiringPiSetup() < 0) {
        fprintf(stderr, "wiringPiSetup error!!!\n");
        return -1;
    }
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if (fd < 0) {
        fprintf(stderr, "wiringPi I2C Setup error!!!");
        return -1;
    }
    if (argc == 1) {
        // Keine Parameterübergabe. Hilfe anzeigen
        return showHelp(argv, &config);
    }
    const char* script = config.system.readallCmd;
    float battVoltage = getBatteryVoltage(script);
    float supplyMaxVoltage = 0.2f;
    supplyMaxVoltage = battVoltage;
    float supplyMaxCurrent = 0.2f;
    supplyMaxCurrent = config.grid.supplyMaxCurrent;
    float supplyLoadPower = 0.2f;
    supplyLoadPower = supplyMaxVoltage * supplyMaxCurrent;
    float supplyLoadTimeSec = 0.f;
    float supplyLoadWattStunden = 0.f;
    int opt;
    int lockFd = -1;
    // Linux Lockfile
    const char* lockFile = config.system.lockFilePath;
    // wertet Parameter aus
    while ((opt = getopt(argc, argv, "hsw:")) != -1) {
        switch (opt) {
            // ==========================================
            // Hilfe
            // ==========================================
            case 'h':
                return showHelp(argv, &config);
            // ==========================================
            // Stop Timer
            // ==========================================
            case 's': {
                const char* pidFilePath = config.system.PIDFilePath;
                // ==========================================
                // Prüfen ob PID Datei existiert
                // ==========================================
                if (access(pidFilePath, F_OK) != 0) {
                    printf("\n\e[0;31m  Kein aktiver Ladevorgang!\n\n");
                    return 1;
                }
                // ==========================================
                // PID Datei öffnen
                // ==========================================
                FILE* pidFile = fopen(pidFilePath, "r");
                if (pidFile == NULL) {
                    perror("fopen");
                    return 1;
                }
                int pid = 0;
                // PID auslesen
                if (fscanf(pidFile, "%d", &pid) != 1) {
                    fclose(pidFile);
                    printf("\n\e[0;31m  PID konnte nicht gelesen werden!\n\n");
                    return 1;
                }
                fclose(pidFile);
                // ==========================================
                // Prozess beenden
                // ==========================================
                sprintf(command, "kill %d", pid);
                int ret = system(command);
                if (ret != 0) {
                    printf("\n\e[0;31m  Prozess konnte nicht beendet werden!\n\n");
                } else {
                    printf("\n\e[0;31m  Prozess %d wurde beendet.\n\n", pid);
                }
                // ==========================================
                // Relais ausschalten
                // ==========================================
                setBit(1, 1);
                sleep(5);
                setBit(0, 1);
                // ==========================================
                // PID Datei & lockfile löschen
                // ==========================================
                unlink(pidFilePath);
                unlink(lockFile);
                return 0;
            }
            // ==========================================
            // Laden starten
            // ==========================================
            case 'w':
                // ==========================================
                // Lockfile öffnen
                // ==========================================
                lockFd = open(lockFile, O_CREAT | O_RDWR, 0666);
                if (lockFd < 0) {
                    perror("open");
                    return 1;
                }
                // ==========================================
                // Lock setzen
                // ==========================================
                if (flock(lockFd, LOCK_EX | LOCK_NB) < 0) {
                    printf("\n\e[0;31m  Es wird bereits aus dem Netz geladen!\n\n");
                    close(lockFd);
                    return 1;
                }
                // zu ladende Wattstunden aus Parameter speichern
                supplyLoadWattStunden = atof(optarg);
                if (!isNumeric(optarg)) {
                    printf("\n\e[0;31m  Der eingegebene Parameter ist keine Zahl!\n\n");
                    return 1;
                }
                if (supplyLoadWattStunden < config.grid.supplyMinLoadWh) {
                    printf("\n\e[0;31m  Der minimale Wert für -w beträgt %d\n\n", config.grid.supplyMinLoadWh);
                    return 1;
                }
                if (supplyLoadWattStunden > config.grid.supplyMaxLoadWh) {
                    printf("\n\e[0;31m  Der maximale Wert für -w beträgt %d\n\n", config.grid.supplyMaxLoadWh);
                    return 1;
                }
                supplyLoadTimeSec = supplyLoadWattStunden / (supplyLoadPower / 3600);
                char timeString[16];
                formatSecondsToHMS((int)supplyLoadTimeSec, timeString, sizeof(timeString));
                printf("\n");
                printf("  %-26s %8.2f V\n", "Ladespannung (U):", supplyMaxVoltage);
                printf("  %-26s %8.2f A\n", "LadestroPIDFilePathm (I):", supplyMaxCurrent);
                printf("  %-26s %8.0f Wh\n", "Ladekapazität:", supplyLoadWattStunden);
                printf("  %-26s %8.2f W\n", "Ladeleistung pro Stunde:", supplyLoadPower);
                printf("  %-26s %6.0f Sek\n", "Errechnete Ladezeit:", supplyLoadTimeSec);
                printf("  %-26s %10s\n", "Entspricht (Zeit):", timeString);
                // ==========================================
                // Relais einschalten
                // ==========================================
                setBit(0, 0);
                sleep(5);
                setBit(1, 0);
                // ==========================================
                // Hintergrundprozess starten
                // ==========================================
                pid_t pid = fork();
                if (pid < 0) {
                    perror("fork");
                    flock(lockFd, LOCK_UN);
                    close(lockFd);
                    return 1;
                }
                // ==========================================
                // Kindprozess
                // ==========================================
                if (pid == 0) {
                    setsid();
                    sleep((int)supplyLoadTimeSec);
                    // Batterie Relais ausschalten
                    setBit(1, 1);
                    sleep(3);
                    // Netzanschluss Relais ausschalten
                    setBit(0, 1);
                    // Lock freigeben
                    flock(lockFd, LOCK_UN);
                    close(lockFd);
                    unlink(config.system.PIDFilePath);
                    unlink(config.system.lockFilePath);
                    exit(0);
                }
                // ==========================================
                // Elternprozess
                // ==========================================
                FILE* pidFile = fopen(config.system.PIDFilePath, "w");
                if (pidFile != NULL) {
                    fprintf(pidFile, "%d\n", pid);
                    fclose(pidFile);
                }

                printf("  %-26s %10d\n", "Prozess ID:", pid);
                printf("\n");
                close(lockFd);
                return 0;

            case ':':
                printf(
                    "\n\e[0;31m"
                    "  Dieser Parameter benötigt einen Wert!\n\n");
                return 1;

            case '?':
                return showHelp(argv, &config);
        }
    }
    return 0;
}
