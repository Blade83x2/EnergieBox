#include <stdio.h>
#include <stdlib.h>         // atoi()
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mymcp23017.h"
#include "iniparse.h"
#include <unistd.h>         // sleep()
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Funktionen vordeklarieren
static int handler(void* config, const char* section, const char* name, const char* value);
int getElkoState(int relais, void* config);
int getRestPower(void * config);
int getCurrentPower(void * config);
int getDevicePower(int relais, void * config);
int getBit(int Port);
void setBit(int Port, int Status);
bool checkMainParameter(char* paramName, int number, void* config);
int showHelp(char**argv, void* config);
int getRestPower(void * config);
char* readStdinLine();
char* Trim(char* str);
void getDataForConfigFile(int relais, void* config);
int isValidName(const char* name);
int isValidNumber(const char* str, int maxNumber);
int isValidTime(const char* timeStr);

// 12V Setup
typedef struct {
    const char* traceTxtFilePath;
} system_setup;

// MCP Setup
typedef struct {
    int address;
    int numberOfRelaisActive;
    int maxPConverter;
    int maxPMicroController;
    const char* traceTxtFilePath;
} mcp_setup;

// Relais Struktur
typedef struct {
    const char* name;
    bool activateOnStart;
    int eltakoState;
    int pMax;
} relais_config;

typedef struct {
    system_setup system;
    mcp_setup mcp;
    relais_config r[16];
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
    if (MATCH("system", "traceTxtFilePath")) {
        pconfig->system.traceTxtFilePath = strdup(value);
    }
    else if (MATCH("mcp", "address")) {
        pconfig->mcp.address = atoi(value);
    }
    else if (MATCH("mcp", "numberOfRelaisActive")) {
        pconfig->mcp.numberOfRelaisActive = atoi(value);
    }
    else if (MATCH("mcp", "maxPConverter")) {
        pconfig->mcp.maxPConverter = atoi(value);
    }
    else if (MATCH("mcp", "maxPMicroController")) {
        pconfig->mcp.maxPMicroController = atoi(value);
    }
    else {
        // Prüfe ob section "Relais X" ist, wobei X von 1 bis 16
        int relaisNum = 0;
        if (sscanf(section, "Relais %d", &relaisNum) == 1) {
            if (relaisNum >= 1 && relaisNum <= 16) {
                int idx = relaisNum - 1;
                if (strcmp(name, "name") == 0) {
                    strcpy(deviceNames[idx], strdup(value));
                }
                else if (strcmp(name, "activateOnStart") == 0) {
                    strcpy(deviceActiveOnStart[idx], value);
                }
                else if (strcmp(name, "eltakoState") == 0) {
                    pconfig->r[idx].eltakoState = atoi(value);
                }
                else if (strcmp(name, "pMax") == 0) {
                    pconfig->r[idx].pMax = atoi(value);
                }
                else {
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
int getElkoState(int relais, void* config) {
    configuration* pconfig = (configuration*)config;
    if (relais < 1 || relais > 16) return 0;
    return pconfig->r[relais - 1].eltakoState;
}

// Gibt gesamten Watt Verbrauch zurück
int getCurrentPower(void * config) {
    configuration* pconfig = (configuration*)config;
    int watt = pconfig->mcp.maxPMicroController;
    for (int i = 0; i < 16; i++) {
        if (pconfig->r[i].eltakoState == 1) {
            watt += pconfig->r[i].pMax;
        }
    }
    return watt;
}

// Gibt benötigte Watt vom einem Gerät zurück
int getDevicePower(int relais, void * config) {
    configuration* pconfig = (configuration*)config;
    if (relais >= 1 && relais <= 16) {
        return pconfig->r[relais - 1].pMax;
    }
    return 0;
}

// Gibt verfügbaren Watt Restwert zurück
int getRestPower(void * config) {
    configuration* pconfig = (configuration*)config;
    int watt = pconfig->mcp.maxPMicroController;
    for (int i = 0; i < 16; i++) {
        if (pconfig->r[i].eltakoState == 1) {
            watt += pconfig->r[i].pMax;
        }
    }
    return pconfig->mcp.maxPConverter - watt;
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
    return -1; // Nicht gefunden
}


// Programmstart
int main(int argc, char**argv) { 
    configuration config;
    if (ini_parse("/Energiebox/12V/config.ini", handler, &config) < 0) { fprintf(stderr, "Can't load '/Energiebox/12V/config.ini\n"); return 1; }
    if(wiringPiSetup()<0) { fprintf(stderr, "wiringPiSetup error!!!\n"); return -1; }
    mcp_begin(config.mcp.address);
    fd = wiringPiI2CSetup(MCP23017_ADDRESS | i2caddr);
    if(fd <0) { fprintf(stderr, "wiringPi I2C Setup error!!!"); return -1; }
    // Keine Parameterübergabe. Liste anzeigen was geschaltet ist
    if(argc == 1) {
         system("clear");
        int ladezustand = get_battery_percentage(config.system.traceTxtFilePath);
        printf("\n\e[30;47m ID      %4dW  12V Gerätename      %3d%    \e[0m\n", getCurrentPower(&config), ladezustand);
        for(int x=1; x<=config.mcp.numberOfRelaisActive; x++) {
              printf("\033[1;97m %2d---->%s %4d%s \t%s  \e[0m\n", x, ((getElkoState(x, &config)==0)?"\e[0;31m":"\e[0;32m"),(getDevicePower(x, &config)), "W", deviceNames[x-1] );
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
        // Wenn Konfiguration aufgerufen wird z.B. mit 12V -set 3
        if (strcmp(argv[1], "-set") == 0)  {
            if (   atoi(argv[2]) > 0    && atoi(argv[2]) <= config.mcp.numberOfRelaisActive) 
            {
                // Abfrage für neue Daten starten, speichern und Liste anzeigen
                getDataForConfigFile(atoi(argv[2]), &config);
                return 0;
            }
        }
        if(!checkMainParameter("relaisNumber", atoi(argv[1]), &config) || !checkMainParameter("relaisZustand", atoi(argv[2]), &config)) {
            return showHelp(argv, &config);
        }
        // wenn gewünschter relaiszustand und config stand gleich sind, nix machen
        if(atoi(argv[2]) != getElkoState(atoi(argv[1]), &config)){ 
            // wenn eingeschaltet wird
            if(atoi(argv[2])==1) { 
                // falls bereits an ist, nichts machen
                if(getElkoState(atoi(argv[1]), &config)==0){
                    // prüfen ob DC Konverter genug Leistung hat
                    if(getRestPower(&config) >= getDevicePower(atoi(argv[1]), &config) && getDevicePower(atoi(argv[1]), &config) <= config.mcp.maxPConverter) {
                         // Relais schalten 
                         setBit(atoi(argv[1])-1, atoi(argv[2])==1?0:1); // Relais einschalten 
                         // elkoState in config.ini schreiben
                         sprintf(command, "bash /Energiebox/12V/setIni.sh %d %d", atoi(argv[1]), atoi(argv[2]));
                         system(command);
                         sleep(0.6);
                         system("clear && 12V");
                    }
                     else {
                        // Nicht genug Watt verfügbar für neues Gerät
                        printf("\e[0;31mDas Gerät benötigt %d Watt aber es sind nur %d Watt verfügbar! Andere Geräte ausschalten..!?\n", getDevicePower(atoi(argv[1]), &config), getRestPower(&config));
                        return 1;
                    }
                }
            }
            else {
                // wenn ausgeschaltet wird
                // Relais ausschalten
                setBit(atoi(argv[1])-1, 1); // Relais ausschalten 
                //  elkoState in config.ini schreiben
                sprintf(command, "bash /Energiebox/12V/setIni.sh %d %d", atoi(argv[1]), atoi(argv[2]));
                system(command);
                sleep(0.6);
                system("clear && 12V");
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
                // falls bereits an ist, nichts machen
                if(getElkoState(atoi(argv[1]), &config)==0){
                    // prüfen ob DC Konverter  genug Leistung hat
                    if(getRestPower(&config) >= getDevicePower(atoi(argv[1]), &config) && getDevicePower(atoi(argv[1]), &config) <= config.mcp.maxPConverter) {
                         // impuls für Elko 
                         setBit(atoi(argv[1])-1, atoi(argv[2])==1?0:1); // Relais einschalten 
                         //  elkoState in config.ini schreiben
                         sprintf(command, "bash /Energiebox/12V/setIni.sh %d %d", atoi(argv[1]), atoi(argv[2]));
                         system(command);
                         sleep(0.6);
                     }
                     else {
                        // Nicht genug Watt verfügbar für neues Gerät
                        printf("\e[0;31mDas Gerät benötigt %d Watt aber es sind nur %d Watt verfügbar! Andere Geräte ausschalten..!?\n", getDevicePower(atoi(argv[1]), &config), getRestPower(&config));
                        return 1;
                    }
                }
            }
            else {
                // wenn ausgeschaltet wird 
                //  elkoState in config.ini schreiben
                sprintf(command, "bash /Energiebox/12V/setIni.sh %d %d", atoi(argv[1]), atoi(argv[2]));
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

// Prüft ob Parameter richtige Syntax hat
bool checkMainParameter(char* paramName, int number, void* config) {
    configuration* pconfig = (configuration*)config;
    if (strcmp(paramName, "relaisNumber") == 0)         { if ( number < 1 || number > pconfig->mcp.numberOfRelaisActive ) return false; } 
    else if (strcmp(paramName, "relaisZustand") == 0)   { if ( number < 0 || number > 1 ) return false; } 
    else if (strcmp(paramName, "relaisTime") == 0)      { if ( number < 0 || number > 84600 ) return false; } 
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
    // HH
    if (!isdigit(timeStr[0]) || !isdigit(timeStr[1])) return 0;
    // MM
    if (!isdigit(timeStr[3]) || !isdigit(timeStr[4])) return 0;
    int hh = (timeStr[0]-'0')*10 + (timeStr[1]-'0');
    int mm = (timeStr[3]-'0')*10 + (timeStr[4]-'0');
    if (hh < 0 || hh > 23) return 0;
    if (mm < 0 || mm > 59) return 0;
    return 1;
}

void getDataForConfigFile(int relais, void* config) {
    configuration* pconfig = (configuration*)config;
    system("clear");
    printf("Neue Gerätekonfiguration für Relais Nr. -> %d erstellen:\n\n", relais);
    char *strname = NULL;
    char *strpMax = NULL;
    char *stractivateOnStart = NULL;
    char *autoStart = NULL;
    char *autoStop = NULL;
    while (1) {
        printf(" -> Neue Bezeichnung eingeben (leer lassen für deaktivieren): ");
        char* input = readStdinLine();
        char* trimmed = Trim(input);
        if (strlen(trimmed) == 0) {
            free(input);
            strname = strdup("-");
            strpMax = strdup("0");
            stractivateOnStart = strdup("false");
            autoStart = strdup("-");
            autoStop = strdup("-");
            break;
        }
        else if (!isValidName(trimmed)) {
            printf("    Erlaubt: 1-20 Zeichen, Leerzeichen, (a-zA-Z0-9),_+-,.ßäöüÄÖÜ\n");
            free(input);
            continue;
        }
        else {
            strname = strdup(trimmed);
            free(input);
            break;
        }
    }
    if (strcmp(strname, "-") != 0) {
        while (1) {
            printf(" -> Verbrauch in Watt (1-%d): ", (pconfig->mcp.maxPConverter - pconfig->mcp.maxPMicroController));
            char* input = readStdinLine();
            char* trimmed = Trim(input);
            if (!isValidNumber(trimmed, (pconfig->mcp.maxPConverter - pconfig->mcp.maxPMicroController))) {
                printf("    Ungültige Zahl! Erlaubt: 1-%d\n", (pconfig->mcp.maxPConverter - pconfig->mcp.maxPMicroController));
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
        if (trimmed[0] == 'J' || trimmed[0] == 'j') {
            stractivateOnStart = strdup("true");
        } else {
            stractivateOnStart = strdup("false");
        }
        free(input);
        // Automatische Schaltung täglich
        while (1) {
            printf(" -> Soll dieses Gerät automatisch starten täglich? (HH:MM, leer für nein): ");
            char* input = readStdinLine();
            char* trimmed = Trim(input);
            if (strlen(trimmed) == 0) {
                autoStart = strdup("-");
                autoStop = strdup("-");
                free(input);
                break;
            }
            else if (!isValidTime(trimmed)) {
                printf("    Ungültiges Zeitformat! Bitte HH:MM im 24h-Format eingeben (z.B. 22:10)!\n");
                free(input);
                continue;
            }
            else {
                autoStart = strdup(trimmed);
                free(input);

                while (1) {
                    printf(" -> Wann soll das Gerät ausgeschaltet werden? (HH:MM): ");
                    char* input2 = readStdinLine();
                    char* trimmed2 = Trim(input2);
                    if (strlen(trimmed2) == 0) {
                        printf("    Bitte eine gültige Uhrzeit eingeben (nicht leer)!\n");
                        free(input2);
                        continue;
                    }
                    else if (!isValidTime(trimmed2)) {
                        printf("    Ungültiges Zeitformat! Bitte HH:MM im 24h-Format eingeben (z.B. 06:30)!\n");
                        free(input2);
                        continue;
                    }
                    else if (strcmp(autoStart, trimmed2) == 0) {
                        printf("    Einschalt- und Ausschaltzeit dürfen nicht gleich sein!\n");
                        free(input2);
                        continue;
                    }
                    else {
                        autoStop = strdup(trimmed2);
                        free(input2);
                        break;
                    }
                }
                break;
            }
        }
    }
    char command[256];
    snprintf(command, sizeof(command), "bash /Energiebox/12V/setConfig.sh %d '%s' '%s' '%s' '%s' '%s'", relais, strname, stractivateOnStart, strpMax, autoStart, autoStop);
    system(command);
    free(strname);
    free(strpMax);
    free(stractivateOnStart);
    free(autoStart);
    free(autoStop);
    sleep(0.5);
    system("clear && 12V");
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
