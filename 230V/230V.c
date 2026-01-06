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

static int handler(void* config, const char* section, const char* name, const char* value);
int getElkoState(int relais, void* config);
int getRestPower(void* config);
int getCurrentPower(void* config);
int getDevicePower(int relais, void* config);
char* getExecOnStart(int relais);
char* getExecOnStop(int relais);
void setBit(int Port, int Status);
int getBit(int Port);
bool checkMainParameter(char* paramName, int number, void* config);
int showHelp(char** argv, void* config);
char* readStdinLine();
char* Trim(char* str);
void getDataForConfigFile(int relais, void* config);
int isValidName(const char* name);
int isValidNumber(const char* str, int maxNumber);
int isValidTime(const char* timeStr);

// 230V Setup
typedef struct {
    const char* traceTxtFilePath;
} system_setup;

// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
    int maxOutputPower;
    int maxPMicroController;
} mcp_setup;

// Relais Strukturen
typedef struct {
    const char* name;
    bool activateOnStart;
    int pMax;
    int eltakoState;
    const char* execOnStart;
    const char* execOnStop;
} relais_config;

typedef struct {
    system_setup system;
    mcp_setup mcp;
    relais_config r[16];  // Array statt einzelner Felder r1..r16
} configuration;

char command[100];
char deviceNames[16][40];
char deviceActiveOnStart[16][6];
int devicePowerMax[16][4];
int deviceEltakoState[16][1];
char deviceExecOnStart[16][200];
char deviceExecOnStop[16][200];

static int handler(void* config, const char* section, const char* name, const char* value) {
    configuration* pconfig = (configuration*)config;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("system", "traceTxtFilePath")) {
        pconfig->system.traceTxtFilePath = strdup(value);
    } else if (MATCH("mcp", "address")) {
        pconfig->mcp.address = atoi(value);
    } else if (MATCH("mcp", "numberOfRelaisActive")) {
        pconfig->mcp.numberOfRelaisActive = atoi(value);
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
                } else if (strcmp(name, "execOnStart") == 0) {
                    strcpy(deviceExecOnStart[idx], strdup(value));
                } else if (strcmp(name, "execOnStop") == 0) {
                    strcpy(deviceExecOnStop[idx], strdup(value));
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
#undef MATCH

// Gibt gespeicherten Zustandswert von einem Relais zurück
int getElkoState(int relais, void* config) {
    configuration* pconfig = (configuration*)config;
    if (relais < 1 || relais > 16) return 0;
    return pconfig->r[relais - 1].eltakoState;
}

// Gibt verfügbaren Watt Restwert zurück
int getRestPower(void* config) {
    configuration* pconfig = (configuration*)config;
    int watt = 0;
    for (int i = 0; i < 16; i++)
        if (pconfig->r[i].eltakoState == 1) watt += pconfig->r[i].pMax;
    return pconfig->mcp.maxOutputPower - watt;
}

// Gibt gesamten Watt Verbrauch zurück
int getCurrentPower(void* config) {
    configuration* pconfig = (configuration*)config;
    int watt = 0;
    for (int i = 0; i < 16; i++)
        if (pconfig->r[i].eltakoState == 1) watt += pconfig->r[i].pMax;
    return watt;
}

// Gibt benötigte Watt vom Gerät zurück
int getDevicePower(int relais, void* config) {
    configuration* pconfig = (configuration*)config;
    if (relais < 1 || relais > 16) return 0;
    return pconfig->r[relais - 1].pMax;
}

// Gibt Auszuführenden Befehl nach Start zurück
char* getExecOnStart(int relais) {
    char* ret = deviceExecOnStart[relais - 1];
    return ret;
}

// Gibt Auszuführenden Befehl nach Stop zurück
char* getExecOnStop(int relais) {
    char* ret = deviceExecOnStop[relais - 1];
    return ret;
}

// Ermittelt den Ladezustand der Batterie (wird aus txt Datei gelesen)
int get_battery_percentage(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        perror("Datei konnte nicht geöffnet werden");
        return -1;
    }
    const char* prefix = "Batterie: Ladezustand in Prozent = ";
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, prefix, strlen(prefix)) == 0) {
            int percent = -1;
            if (sscanf(line + strlen(prefix), "%d", &percent) == 1) {
                fclose(file);
                return percent;
            }
        }
    }
    fclose(file);
    return -1;  // Nicht gefunden
}

// Programmstart
int main(int argc, char** argv) {
    configuration config;
    if (ini_parse("/Energiebox/230V/config.ini", handler, &config) < 0) {
        printf("Can't load '/Energiebox/230V/config.ini'\n");
        return 1;
    }
    if (wiringPiSetup() < 0) {
        printf("wiringPiSetup error!!!");
        return -1;
    }
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if (fd < 0) {
        printf("wiringPi I2C Setup error!!!");
        return -1;
    }
    // Keine Parameterübergabe. Liste anzeigen was geschaltet ist
    if (argc == 1) {
        // Keine Parameterübergabe. Liste anzeigen was geschaltet ist
        // system("clear");
        int ladezustand = get_battery_percentage(config.system.traceTxtFilePath);
        printf("\n\e[30;47m ID      %4dW  230V Gerätename     %3d%    \e[0m\n", getCurrentPower(&config), ladezustand);
        for (int x = 1; x <= config.mcp.numberOfRelaisActive; x++) {
            printf("\033[1;97m %2d---->%s %4d%s \t%s  \e[0m\n", x, ((getElkoState(x, &config) == 0) ? "\e[0;31m" : "\e[0;32m"), (getDevicePower(x, &config)), "W",
                   deviceNames[x - 1]);
        }
        printf("\n");
    } else if (argc == 2) {  // Nur Relais Nummer übergeben,
                             // wenn relaisNummer nicht valide
        if (!checkMainParameter("relaisNumber", atoi(argv[1]), &config)) {
            return showHelp(argv, &config);
        }
        printf("%d", getElkoState(atoi(argv[1]), &config));  // Nur Relais Nr. übergeben. config.ini auslesen und int auf console printen
    } else if (argc == 3) {
        // Wenn Konfiguration aufgerufen wird z.B. mit 230V -set 3
        if (strcmp(argv[1], "-set") == 0) {
            if (atoi(argv[2]) > 0 && atoi(argv[2]) <= config.mcp.numberOfRelaisActive) {
                // Abfrage für neue Daten starten, speichern und Liste anzeigen
                getDataForConfigFile(atoi(argv[2]), &config);
                return 0;
            }
        }
        if (!checkMainParameter("relaisNumber", atoi(argv[1]), &config) || !checkMainParameter("relaisZustand", atoi(argv[2]), &config)) {
            return showHelp(argv, &config);
        }
        // wenn eingeschaltet wird
        if (atoi(argv[2]) == 1) {
            // falls bereits an ist, nichts machen
            if (getElkoState(atoi(argv[1]), &config) == 0) {
                // prüfen ob genug power da ist
                if (getRestPower(&config) >= getDevicePower(atoi(argv[1]), &config) && getDevicePower(atoi(argv[1]), &config) <= config.mcp.maxOutputPower) {
                    // Relais einschalten
                    setBit(atoi(argv[1]) - 1, 0);
                    //  elkoState in config.ini schreiben
                    sprintf(command, "bash /Energiebox/230V/setIni.sh %d %d", atoi(argv[1]), 1);
                    system(command);
                    char* cmd = getExecOnStart(atoi(argv[1]));
                    if (cmd != NULL) {
                        system(cmd);
                    }
                    sleep(0.6);
                    system("clear && 230V");
                    exit;
                } else {
                    // Nicht genug Watt verfügbar für neues Gerät
                    printf("\e[0;31mDas Gerät benötigt %d Watt aber es sind nur %d Watt verfügbar!\n", getDevicePower(atoi(argv[1]), &config), getRestPower(&config));
                }
            }
        } else {
            // wenn ausgeschaltet wird
            // Relais ausschalten
            setBit(atoi(argv[1]) - 1, 1);
            //  elkoState in config.ini schreiben
            sprintf(command, "bash /Energiebox/230V/setIni.sh %d %d", atoi(argv[1]), 0);
            system(command);
            char* cmd = getExecOnStop(atoi(argv[1]));
            if (cmd != NULL) {
                system(cmd);
            }
            sleep(0.6);
            system("clear && 230V");
        }
    } else if (argc == 4) {
        if (!checkMainParameter("relaisNumber", atoi(argv[1]), &config) || !checkMainParameter("relaisZustand", atoi(argv[2]), &config) ||
            !checkMainParameter("relaisTime", atoi(argv[3]), &config)) {
            return showHelp(argv, &config);
        }
        sleep(atoi(argv[3]));
        // wenn eingeschaltet wird
        if (atoi(argv[2]) == 1) {
            // falls bereits an ist, nichts machen
            if (getElkoState(atoi(argv[1]), &config) == 0) {
                // prüfen ob Spannungswandler genug Watt  für alles liefert
                if (getRestPower(&config) >= getDevicePower(atoi(argv[1]), &config) && getDevicePower(atoi(argv[1]), &config) <= config.mcp.maxOutputPower) {
                    // Relais einschalten
                    setBit(atoi(argv[1]) - 1, 0);
                    //  elkoState in config.ini schreiben
                    sprintf(command, "bash /Energiebox/230V/setIni.sh %d %d", atoi(argv[1]), 1);
                    system(command);
                    char* cmd = getExecOnStart(atoi(argv[1]));
                    if (cmd != NULL) {
                        system(cmd);
                    }
                    sleep(0.6);
                } else {
                    // Nicht genug Watt verfügbar für neues Gerät
                    printf("\e[0;31mDas Gerät benötigt %d Watt aber es sind nur %d Watt verfügbar!\n", getDevicePower(atoi(argv[1]), &config), getRestPower(&config));
                }
            }
        } else {
            // wenn ausgeschaltet wird
            // Relais ausschalten
            setBit(atoi(argv[1]) - 1, 1);
            //  elkoState in config.ini schreiben
            sprintf(command, "bash /Energiebox/230V/setIni.sh %d %d", atoi(argv[1]), 0);
            system(command);
            char* cmd = getExecOnStop(atoi(argv[1]));
            if (cmd != NULL) {
                system(cmd);
            }
            sleep(0.6);
        }
    } else {
        return showHelp(argv, &config);
    }
    return 0;
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

// Liesst Bit von Relais Zustand aus
int getBit(int Port) {
    Port -= 1;
    int reg = (Port > -1 && Port < 8) ? 0x12 : 0x13;
    int PIN = (Port > -1 && Port < 8) ? Port : Port % 8;
    int val = mcp_readRegister(reg);
    return (val & (1 << PIN)) ? 0 : 1;
}

// Prüft ob Parameter richtige Syntax hat
bool checkMainParameter(char* paramName, int number, void* config) {
    configuration* pconfig = (configuration*)config;
    if (strcmp(paramName, "relaisNumber") == 0) {
        if (number < 1 || number > pconfig->mcp.numberOfRelaisActive) return false;
    } else if (strcmp(paramName, "relaisZustand") == 0) {
        if (number < 0 || number > 1) return false;
    } else if (strcmp(paramName, "relaisTime") == 0) {
        if (number < 0 || number > 86400) return false;
    }
    return true;
}

#define MAX_INPUT_LEN 256

// Liest eine ganze Zeile von stdin, gibt malloc'ed String zurück (muss frei sein)
char* readStdinLine() {
    char buffer[MAX_INPUT_LEN];
    if (fgets(buffer, MAX_INPUT_LEN, stdin) == NULL) {
        char* empty = malloc(1);
        if (empty) empty[0] = '\0';
        return empty;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
    char* line = malloc(len);
    if (!line) {
        fprintf(stderr, "Fehler: Speicherallokierung fehlgeschlagen.\n");
        exit(1);
    }
    strcpy(line, buffer);
    return line;
}

// Trimmt Leerzeichen vorne und hinten in-place, gibt Pointer auf Anfang zurück
char* Trim(char* str) {
    if (!str) return NULL;
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return str;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return str;
}

// Prüft, ob der String nur erlaubte Zeichen enthält (a)
int isValidName(const char* name) {
    if (name == NULL || strlen(name) > 20) return 0;
    const char* allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _+-.,äöüÄÖÜß";
    for (int i = 0; name[i] != '\0'; i++) {
        if (strchr(allowed, name[i]) == NULL) {
            return 0;
        }
    }
    return 1;
}

// Prüft, ob str eine gültige Zahl (b) im Bereich 1-5000 ist
int isValidNumber(const char* str, int maxNumber) {
    if (str == NULL || *str == '\0') return 0;
    const char* p = str;
    while (*p) {
        if (!isdigit((unsigned char)*p)) return 0;
        p++;
    }
    int val = atoi(str);
    if (val < 1 || val > maxNumber) return 0;
    return 1;
}

// Prüft Uhrzeit im Format HH:MM (24h)
int isValidTime(const char* timeStr) {
    if (timeStr == NULL) return 0;
    if (strlen(timeStr) != 5) return 0;
    if (timeStr[2] != ':') return 0;
    if (!isdigit(timeStr[0]) || !isdigit(timeStr[1])) return 0;  // HH
    if (!isdigit(timeStr[3]) || !isdigit(timeStr[4])) return 0;  // MM
    int hh = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
    int mm = (timeStr[3] - '0') * 10 + (timeStr[4] - '0');
    if (hh < 0 || hh > 23) return 0;
    if (mm < 0 || mm > 59) return 0;
    return 1;
}

// Fragt Informationen am Bildschurm ab
void getDataForConfigFile(int relais, void* config) {
    configuration* pconfig = (configuration*)config;
    system("clear");
    printf("Gerätekonfiguration für Relais Nr. -> %d bearbeiten:\n\n", relais);
    char* strname = NULL;
    char* strpMax = NULL;
    char* stractivateOnStart = NULL;
    char* autoStart = NULL;
    char* autoStop = NULL;
    char* canStartFromGui = NULL;
    char* execOnStart = NULL;
    char* execOnStop = NULL;
    while (1) {
        printf(" -> Bezeichnung (leer = deaktiviert): ");
        char* input = readStdinLine();
        char* trimmed = Trim(input);
        if (strlen(trimmed) == 0) {
            free(input);
            strname = strdup("-");
            strpMax = strdup("0");
            stractivateOnStart = strdup("false");
            autoStart = strdup("-");
            autoStop = strdup("-");
<<<<<<< HEAD
=======
            canStartFromGui = strdup("1");
            execOnStart = strdup("");
            execOnStop = strdup("");
>>>>>>> b16c7bb (execOnStart & execOnStop implementiert in 12V und 230V. Nun können Bash Befehle bei der Schaltung definiert werden!)
            break;
        } else if (!isValidName(trimmed)) {
            printf("    Ungültig! Erlaubt sind: 1-20 Zeichen,\n    Leerzeichen, (a-zA-Z0-9),_+-,.ßäöüÄÖÜ\n");
            free(input);
            continue;
        } else {
            strname = strdup(trimmed);
            free(input);
            break;
        }
    }
    if (strcmp(strname, "-") != 0) {
        while (1) {
            printf(" -> Leistung (Watt, 1 bis %d): ", pconfig->mcp.maxOutputPower);
            char* input = readStdinLine();
            char* trimmed = Trim(input);
            if (!isValidNumber(trimmed, pconfig->mcp.maxOutputPower)) {
                printf("    Ungültige Zahl! Erlaubt: 1-%d\n", pconfig->mcp.maxOutputPower);
                free(input);
                continue;
            }
            strpMax = strdup(trimmed);
            free(input);
            break;
        }
        // Aktiv beim Start (J/N)
        printf(" -> Beim Anlagenstart aktivieren? (J/N): ");
        char* input = readStdinLine();
        char* trimmed = Trim(input);
        stractivateOnStart = ((trimmed[0] == 'J' || trimmed[0] == 'j') ? strdup("true") : strdup("false"));
        free(input);
        // Automatische Schaltung täglich
        while (1) {
            printf(" -> Soll dieses Gerät automatisch starten\n    täglich? (HH:MM, leer = nein): ");
            char* input = readStdinLine();
            char* trimmed = Trim(input);
            if (strlen(trimmed) == 0) {
                autoStart = strdup("-");
                autoStop = strdup("-");
                free(input);
                break;
            } else if (!isValidTime(trimmed)) {
                printf("    Ungültiges Zeitformat! Bitte HH:MM im\n    24h-Format eingeben (z.B. 22:10)!\n");
                free(input);
                continue;
            } else {
                autoStart = strdup(trimmed);
                free(input);
                while (1) {
                    printf(" -> Und wann soll das Gerät wieder\n    ausgeschaltet werden? (HH:MM): ");
                    char* input2 = readStdinLine();
                    char* trimmed2 = Trim(input2);
                    if (strlen(trimmed2) == 0) {
                        printf("    Bitte eine gültige Uhrzeit\n    eingeben (nicht leer)!\n");
                        free(input2);
                        continue;
                    } else if (!isValidTime(trimmed2)) {
                        printf("    Ungültiges Zeitformat! Bitte\n    HH:MM im 24h-Format eingeben (z.B. 06:30)!\n");
                        free(input2);
                        continue;
                    } else if (strcmp(autoStart, trimmed2) == 0) {
                        printf("    Einschalt- und Ausschaltzeit\n    dürfen nicht gleich sein!\n");
                        free(input2);
                        continue;
                    } else {
                        autoStop = strdup(trimmed2);
                        free(input2);
                        break;
                    }
                }
                break;
            }
        }
        // Soll das Aktivieren dieses Gerätes in der GUI gesperrt werden (J/N)
        printf(" -> Schalten in der GUI verbieten? (J/N): ");
        char* input3 = readStdinLine();
        char* trimmed3 = Trim(input3);
        canStartFromGui = (trimmed3[0] == 'J' || trimmed3[0] == 'j') ? strdup("0") : strdup("1");
        free(input3);

        // Soll ein Befehl beim starten des Gerätes ausgeführt werden?
        printf(" -> Soll ein Befehl nach dem Einschalten ausgeführt werden?: ");
        char* input4 = readStdinLine();
        char* trimmed4 = Trim(input4);
        if (strlen(trimmed4) == 0) {
            execOnStart = strdup("");
        } else {
            execOnStart = strdup(trimmed4);
        }
        free(input4);

        // Soll ein Befehl beim ausschalten des Gerätes ausgeführt werden?
        printf(" -> Soll ein Befehl nach dem Ausschalten ausgeführt werden?: ");
        char* input5 = readStdinLine();
        char* trimmed5 = Trim(input5);
        if (strlen(trimmed5) == 0) {
            execOnStop = strdup("");
        } else {
            execOnStop = strdup(trimmed5);
        }
        free(input5);
    }
    char command[256];
    snprintf(command, sizeof(command), "bash /Energiebox/230V/setConfig.sh %d '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s'", relais, strname, stractivateOnStart, strpMax, autoStart,
             autoStop, canStartFromGui, execOnStart, execOnStop);
    system(command);
    free(strname);
    free(strpMax);
    free(stractivateOnStart);
    free(autoStart);
    free(autoStop);
    free(canStartFromGui);
    free(execOnStart);
    free(execOnStop);
    sleep(0.5);
    system("clear && 230V");
}

// Zeigt Hilfe auf Console an
int showHelp(char** argv, void* config) {
    configuration* pconfig = (configuration*)config;
    printf("\n\e[1;31mFehler: Falsche Parameter!\e[0m\n\n");
    printf("\e[1;33mVerwendung:\e[0m\n");
    printf("  %s \e[0;36m[Zeigt den aktuellen Belegungsplan an]\e[0m\n", argv[0]);
    printf("  %s \e[1;32m<Relaisnummer>\e[0m \e[0;36m[Status von Relais anzeigen (1 bis %d)]\e[0m\n", argv[0], pconfig->mcp.numberOfRelaisActive);
    printf("  %s \e[1;32m<Relaisnummer>\e[0m \e[1;32m<0|1>\e[0m \e[0;36m[Relais an (1) oder aus (0) schalten]\e[0m\n", argv[0]);
    printf("  %s \e[1;32m<Relaisnummer>\e[0m \e[1;32m<0|1>\e[0m \e[1;32m<Sekunden>\e[0m \e[0;36m[Relais nach X Sekunden schalten]\e[0m\n", argv[0]);
    printf("  %s \e[1;32m<Relaisnummer>\e[0m \e[1;32m<0|1>\e[0m \e[1;32m<Sekunden>\e[0m \e[1;35m& disown\e[0m \e[0;36m[Im Hintergrund nach X Sekunden schalten]\e[0m\n", argv[0]);
    printf("  %s \e[1;32m-set <Relaisnummer>\e[0m \e[0;36m[Einstellungsmenü für Relais aufrufen]\e[0m\n\n", argv[0]);
    return -1;
}
