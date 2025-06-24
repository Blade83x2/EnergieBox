/*
 *  Ruft python3 /Energiebox/Tracer/readall.py auf und liest die MPPT-Daten aus.
 *  Prüft, ob die Batterie eine Mindestspannung hat, und startet ggf. das Netzladeprogramm.
 *  Speichert die ausgelesenen Daten in eine MySQL-Datenbank und in trace.txt.
 */

#include "iniparse.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <mysql/mysql.h>
#include <stdexcept>
#include <string>
#include <unistd.h>  // für access() und F_OK

struct MCPSetup {
    int address = 0;
    int numberOfRelaisActive = 0;
};

struct GridSetup {
    float supplyMaxCurrent = 0.0f;
    float supplyMaxVoltage = 0.0f;
    int supplyMinLoadWh = 0;
    int supplyMaxLoadWh = 0;
    float battVoltageStartLoading = 0.0f;
    int loadingCapacityWh = 0;
};
// Strukt
struct Configuration {
    MCPSetup mcp;
    GridSetup grid;
};

void schreibe_zeile_in_datei(const std::string &dateipfad, const std::string &textzeile) {
    std::ofstream datei(dateipfad, std::ios::app);
    if(datei.is_open()) {
        datei << textzeile << std::endl;
    } else {
        std::cerr << "/Energiebox/Tracer/trace: Fehler: Konnte Datei nicht öffnen: " << dateipfad << std::endl;
    }
}

static int
handler(void *config, const char *section, const char *name, const char *value) {
    Configuration *pconfig = static_cast<Configuration *>(config);
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("mcp", "address")) {
        pconfig->mcp.address = std::atoi(value);
    } else if(MATCH("mcp", "numberOfRelaisActive")) {
        pconfig->mcp.numberOfRelaisActive = std::atoi(value);
    } else if(MATCH("grid", "supplyMaxCurrent")) {
        pconfig->grid.supplyMaxCurrent = std::atof(value);
    } else if(MATCH("grid", "supplyMaxVoltage")) {
        pconfig->grid.supplyMaxVoltage = std::atof(value);
    } else if(MATCH("grid", "supplyMinLoadWh")) {
        pconfig->grid.supplyMinLoadWh = std::atoi(value);
    } else if(MATCH("grid", "supplyMaxLoadWh")) {
        pconfig->grid.supplyMaxLoadWh = std::atoi(value);
    } else if(MATCH("grid", "battVoltageStartLoading")) {
        pconfig->grid.battVoltageStartLoading = std::atof(value);
    } else if(MATCH("grid", "loadingCapacityWh")) {
        pconfig->grid.loadingCapacityWh = std::atoi(value);
    } else {
        return 0;
    }
    return 1;
}

class BatteryController {
  private:
    Configuration config;
    const std::string configPath = "/Energiebox/Grid/config.ini";
    const std::string readallCmd = "python3 /Energiebox/Tracer/readall.py";
    const std::string voltagePrefix = "Batterie: Aktuelle Spannung in Volt = ";
    const std::string outputPath = "/Energiebox/Tracer/trace.txt";
    bool
    loadConfig() {
        return ini_parse(configPath.c_str(), handler, &config) >= 0;
    }

    float
    parseVoltageLine(const std::string &line) {
        auto pos = line.find(voltagePrefix);
        if(pos != std::string::npos) {
            std::string valStr = line.substr(pos + voltagePrefix.size());
            float voltage = 0.0f;
            if(sscanf(valStr.c_str(), "%fV", &voltage) == 1) {
                return voltage;
            }
        }
        return -1.0f;
    }
    void
    triggerLoad() {
        std::string cmd = "/Energiebox/Grid/grid -w " + std::to_string(config.grid.loadingCapacityWh) + " &";
        int ret = system(cmd.c_str());
        if(ret == -1) {
            std::cerr << "/Energiebox/Tracer/trace: Fehler beim Starten des Grid Programm\n";
        } else {
            std::cout << "Grid-Ladevorgang gestartet (" << config.grid.loadingCapacityWh
                      << " Wh)\n";
        }
    }
    void
    speichereInDatenbank(float pv_volt,
                         float pv_ampere,
                         float pv_power,
                         float batt_volt,
                         float batt_ampere,
                         float batt_power, int batt_soc, float generated_power,
                         int grid_load_active) {
        MYSQL *conn = mysql_init(nullptr);
        if(!conn) {
            std::cerr << "/Energiebox/Tracer/trace: MySQL-Init fehlgeschlagen\n";
            return;
        }
        if(!mysql_options(conn, MYSQL_READ_DEFAULT_FILE, "/home/box/.mysql_energiebox.cfg")) {
            if(mysql_real_connect(conn, nullptr, nullptr, nullptr, nullptr, 0, nullptr, 0)) {
                unsigned int timestamp = static_cast<unsigned int>(std::time(nullptr));
                std::string sql = "INSERT INTO messwerte (timestamp, pv_volt, pv_ampere, pv_power, "
                                  "batt_volt, batt_ampere, batt_power, batt_soc, generated_power, "
                                  "grid_load_active) VALUES (" +
                                  std::to_string(timestamp) + ", " + std::to_string(pv_volt) + ", " + std::to_string(pv_ampere) + ", " + std::to_string(pv_power) + ", " + std::to_string(batt_volt) + ", " + std::to_string(batt_ampere) + ", " + std::to_string(batt_power) + ", " + std::to_string(batt_soc) + ", " + std::to_string(generated_power) + ", " + std::to_string(grid_load_active) + ")";
                if(mysql_query(conn, sql.c_str())) {
                    std::cerr << "/Energiebox/Tracer/trace: MySQL INSERT fehlgeschlagen: "
                              << mysql_error(conn) << "\n";
                } else {
                    std::cout << "Messwerte erfolgreich gespeichert.\n";
                }
                mysql_close(conn);
            } else {
                std::cerr << "/Energiebox/Tracer/trace: MySQL-Verbindung fehlgeschlagen: "
                          << mysql_error(conn) << "\n";
                mysql_close(conn);
            }
        } else {
            std::cerr << "/Energiebox/Tracer/trace: Konnte MySQL-Konfig nicht laden!\n";
        }
    }

  public:
    bool
    run() {
        if(!loadConfig()) {
            std::cerr << "/Energiebox/Tracer/trace: Fehler: Konnte Config nicht laden: "
                      << configPath << "\n";
            return false;
        }
        std::array<char, 256> buffer;
        FILE *pipe = popen(readallCmd.c_str(), "r");
        if(!pipe) {
            std::cerr << "/Energiebox/Tracer/trace: Fehler: \"Konnte python3 "
                         "/Energiebox/Tracer/readall.py\" nicht ausführen\n";
            return false;
        }
        std::ofstream outfile(outputPath);
        if(!outfile.is_open()) {
            std::cerr << "/Energiebox/Tracer/trace: Fehler: Konnte trace.txt nicht öffnen\n";
            schreibe_zeile_in_datei("/Energiebox/error.log",
                                    "/Energiebox/Tracer/trace: Fehler: Konnte "
                                    "/Energiebox/Tracer/trace.txt nicht öffnen!");
            pclose(pipe);
            return false;
        }
        float pv_volt = 0, pv_ampere = 0, pv_power = 0;
        float batt_volt = 0, batt_ampere = 0, batt_power = 0;
        int batt_soc = 0;
        float generated_power = 0.0f;
        bool loadTriggered = false;
        int grid_load_active = (access("/Energiebox/Grid/isLoading.lock", F_OK) == 0) ? 1 : 0;
        while(fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            std::string line(buffer.data());
            outfile << line;
            sscanf(line.c_str(), "PV Array: Aktuelle Spannung in Volt = %fV", &pv_volt);
            sscanf(line.c_str(), "PV Array: Aktueller Strom in Ampere = %fA", &pv_ampere);
            sscanf(line.c_str(), "PV Array: Aktuelle Leistung in Watt = %fW", &pv_power);
            sscanf(line.c_str(), "Batterie: Aktuelle Spannung in Volt = %f", &batt_volt);
            sscanf(line.c_str(), "Batterie: Derzeitiger Ladestrom in Ampere = %fA", &batt_ampere);
            sscanf(line.c_str(), "Batterie: Derzeitige Ladeleistung in Watt = %fW", &batt_power);
            sscanf(line.c_str(), "Batterie: Ladezustand in Prozent = %d%%", &batt_soc);
            sscanf(line.c_str(), "PV Array: Generierte Energie heute = %fkWh", &generated_power);
            if(line.find(voltagePrefix) == 0) {
                float voltage = parseVoltageLine(line);
                std::cout << "Aktuelle Batteriespannung: " << voltage << " V\n";
                if(grid_load_active == 0) {
                    if(voltage < config.grid.battVoltageStartLoading) {
                        std::cout << "Niedrige Spannung erkannt, lade Grid...\n";
                        triggerLoad();
                        loadTriggered = true;
                    } else {
                        std::cout << "Spannung ok, kein Laden notwendig.\n";
                    }
                } else {
                    std::cout << "Batterie wird bereits geladen!\n";
                }
            }
        }
        outfile.close();
        pclose(pipe);
        speichereInDatenbank(pv_volt,
                             pv_ampere,
                             pv_power,
                             batt_volt,
                             batt_ampere,
                             batt_power,
                             batt_soc,
                             generated_power,
                             grid_load_active);
        return loadTriggered;
    }
};

int main() {
    BatteryController controller;
    if(!controller.run()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
