/*
 * Mit
 *  journalctl -fu rcshutdown.service
 * kann der Service debugt werden
 */

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
#include "mysql_wrapper.h"
#include <signal.h>

// system Setup
typedef struct {
    const char *mysqlCfgPath;
    const char *PIDFilePath;
    const char *lockFilePath;
} system_setup;

// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
} mcp_setup;

// Relais Struktur
typedef struct {
    int eltakoState;
} relais_config;

typedef struct {
    system_setup system;
    mcp_setup mcp;
    relais_config r[16];
} configuration;

static int handler(void *config, const char *section, const char *name, const char *value) {
    configuration *pconfig = (configuration *)config;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("system", "mysqlCfgPath")) {
        pconfig->system.mysqlCfgPath = strdup(value);
    } else if (MATCH("system", "PIDFilePath")) {
        pconfig->system.PIDFilePath = strdup(value);
    } else if (MATCH("system", "lockFilePath")) {
        pconfig->system.lockFilePath = strdup(value);
    } else if (MATCH("mcp", "address")) {
        pconfig->mcp.address = atoi(value);
    } else if (MATCH("mcp", "numberOfRelaisActive")) {
        pconfig->mcp.numberOfRelaisActive = atoi(value);
    } else {
        int relaisNum = 0;
        if (sscanf(section, "Relais %d", &relaisNum) == 1) {
            if (relaisNum >= 1 && relaisNum <= 16) {
                int idx = relaisNum - 1;
                if (strcmp(name, "eltakoState") == 0) {
                    pconfig->r[idx].eltakoState = atoi(value);
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

// Gibt gespeicherten Zustandswert von einem Relais zurück
int getElkoState(int relais, void *config) {
    configuration *pconfig = (configuration *)config;
    if (relais < 1 || relais > 16) return 0;
    return pconfig->r[relais - 1].eltakoState;
}

// Schreibt Schaltung in die Datenbank
void insertSchaltung12V(MYSQL *conn, int relais, char *zustand) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO schaltungen_12v (relais, zustand) VALUES (%d, '%s')", relais, zustand);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL Fehler: %s\n", mysql_error(conn));
        FILE *fp = fopen("/Energiebox/error.log", "a");
        if (fp) {
            fprintf(fp, "MySQL Fehler: %s\n", mysql_error(conn));
            fclose(fp);
        }
    }
}
// Schreibt Schaltung in die Datenbank
void insertSchaltung230V(MYSQL *conn, int relais, char *zustand) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO schaltungen_230v (relais, zustand) VALUES (%d, '%s')", relais, zustand);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL Fehler: %s\n", mysql_error(conn));
        FILE *fp = fopen("/Energiebox/error.log", "a");
        if (fp) {
            fprintf(fp, "MySQL Fehler: %s\n", mysql_error(conn));
            fclose(fp);
        }
    }
}

void insertSchaltunggrid(MYSQL *conn, char *zustand) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO schaltungen_grid (zustand) VALUES ('%s')", zustand);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "MySQL Fehler: %s\n", mysql_error(conn));
        FILE *fp = fopen("/Energiebox/error.log", "a");
        if (fp) {
            fprintf(fp, "MySQL Fehler: %s\n", mysql_error(conn));
            fclose(fp);
        }
    }
}

// System Kommandos StringactivateOnStart
char command[100];

int main(int argc, char **argv) {
    configuration config;

    // sprintf(command, "bash /Energiebox/Grid/grid -s");
    // system(command);

    /////////////////////
    //// 12 Volt  ///////
    /////////////////////
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
    // Datenbank Setup
    DBConfig mysqlconfig = {0};
    if (!load_db_config(config.system.mysqlCfgPath, &mysqlconfig)) {
        fprintf(stderr, "%s konnte nicht geladen werden", config.system.mysqlCfgPath);
        return -1;
    }
    MYSQL *conn = db_connect(&mysqlconfig);
    if (!conn) {
        fprintf(stderr, "DB Verbindung fehlgeschlagen. Datenbankdaten in mysql_energiebox.cfg prüfen!");
        return -1;
    }
    if (mysql_ping(conn) != 0) {
        fprintf(stderr, "MySQL nicht erreichbar\n");
        exit(EXIT_FAILURE);
    }
    mcp_initReg();
    for (int i = 0; i < config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i, 1);

        // wenn das Relais eingeschaltet war, in Datenbank das ausschalten notieren
        if (getElkoState(i + 1, &config) == 1) {
            insertSchaltung12V(conn, i + 1, "aus");
        }

        sprintf(command, "bash /Energiebox/12V/setIni.sh %d %d", (i + 1), 0);
        system(command);
        sleep(0.7);
    }
    ////////////////////
    ///// 230 Volt  ////
    ////////////////////
    // 230V config laden
    if (ini_parse("/Energiebox/230V/config.ini", handler, &config) < 0) {
        fprintf(stderr, "Can't load '/Energiebox/230V/config.ini\n");
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
    // Datenbank cocnfig für 230V laden
    DBConfig mysqlconfig230 = {0};
    if (!load_db_config(config.system.mysqlCfgPath, &mysqlconfig230)) {
        fprintf(stderr, "%s konnte nicht geladen werden", config.system.mysqlCfgPath);
        return -1;
    }
    MYSQL *conn230 = db_connect(&mysqlconfig230);
    if (!conn230) {
        fprintf(stderr, "DB Verbindung fehlgeschlagen. Datenbankdaten in mysql_energiebox.cfg prüfen!");
        return -1;
    }
    if (mysql_ping(conn230) != 0) {
        fprintf(stderr, "MySQL nicht erreichbar\n");
        exit(EXIT_FAILURE);
    }
    // Register MCP Bits
    mcp_initReg();
    for (int i = 0; i < config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        // alle RelaiBits ausschalten
        mcp_digitalWrite(i, 1);
        // wenn das Relais eingeschaltet war, in Datenbank das ausschalten notieren
        if (getElkoState(i + 1, &config) == 1) {
            insertSchaltung230V(conn, i + 1, "aus");
        }
        sprintf(command, "bash /Energiebox/230V/setIni.sh %d %d", (i + 1), 0);
        system(command);
        sleep(0.7);
    }
    ////////////////////
    ///// GRID      ////
    ////////////////////

    if (ini_parse("/Energiebox/Grid/config.ini", handler, &config) < 0) {
        fprintf(stderr, "Can't load '/Energiebox/Grid/config.ini\n");
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

    // Datenbank config für grid laden
    DBConfig mysqlconfiggrid = {0};
    if (!load_db_config(config.system.mysqlCfgPath, &mysqlconfiggrid)) {
        fprintf(stderr, "%s konnte nicht geladen werden", config.system.mysqlCfgPath);
        return -1;
    }
    MYSQL *conngrid = db_connect(&mysqlconfiggrid);
    if (!conngrid) {
        fprintf(stderr, "DB Verbindung fehlgeschlagen. Datenbankdaten in %s prüfen!", config.system.mysqlCfgPath);
        return -1;
    }
    mcp_initReg();
    for (int i = 0; i < config.mcp.numberOfRelaisActive; i++) {
        mcp_pinMode(i, 0);
        mcp_digitalWrite(i, 1);
        sleep(0.7);
    }
    // prüfen ob am laden ist
    // wenn ja prozess beenden
    const char *PIDFilePath = config.system.PIDFilePath;
    if (access(PIDFilePath, F_OK) == 0) {
        FILE *fp = fopen(PIDFilePath, "r");
        if (fp) {
            int pid;
            fscanf(fp, "%d", &pid);
            fclose(fp);
            printf("grid PID gefunden: %d\n", pid);
            if (kill(pid, SIGTERM) == 0) {
                printf("Prozess beendet\n");
            } else {
                printf("Kein Zugriff auf Prozess\n");
            }
            printf("PID File löschen und auf stop setzten in Datenbank\n");
            sprintf(command, "rm -f %s", PIDFilePath);
            system(command);
            sprintf(command, "rm -f %s", config.system.lockFilePath);
            system(command);
            insertSchaltunggrid(conngrid, "aus");
        }
    }

    return 0;
}
