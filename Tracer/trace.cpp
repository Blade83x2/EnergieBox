#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <cstdlib>
#include <cstring>

// INI-Parsing (du kannst deine eigene Lösung hier integrieren)
#include "iniparse.h" // wie gehabt, handler-Funktion bleibt in C

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

struct Configuration {
    MCPSetup mcp;
    GridSetup grid;
};

static int handler(void* config, const char* section, const char* name, const char* value) {
    Configuration* pconfig = static_cast<Configuration*>(config);
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("mcp", "address")) {  pconfig->mcp.address = std::atoi(value); }
    else if(MATCH("mcp", "numberOfRelaisActive")) { pconfig->mcp.numberOfRelaisActive = std::atoi(value); }
    else if(MATCH("grid", "supplyMaxCurrent")) { pconfig->grid.supplyMaxCurrent = std::atof(value); }
    else if(MATCH("grid", "supplyMaxVoltage")) { pconfig->grid.supplyMaxVoltage = std::atof(value); }
    else if(MATCH("grid", "supplyMinLoadWh")) { pconfig->grid.supplyMinLoadWh = std::atoi(value); }
    else if(MATCH("grid", "supplyMaxLoadWh")) { pconfig->grid.supplyMaxLoadWh = std::atoi(value); }
    else if(MATCH("grid", "battVoltageStartLoading")) { pconfig->grid.battVoltageStartLoading = std::atof(value); }
    else if(MATCH("grid", "loadingCapacityWh")) { pconfig->grid.loadingCapacityWh = std::atoi(value); }
    else { return 0; }
    return 1;
}

class BatteryController {
private:
    Configuration config;
    const std::string configPath = "/Energiebox/Grid/config.ini";
    const std::string readallCmd = "python3 /Energiebox/Tracer/readall.py";
    const std::string voltagePrefix = "Batterie: Aktuelle Spannung in Volt = ";

    bool loadConfig() {
        return ini_parse(configPath.c_str(), handler, &config) >= 0;
    }

    float parseVoltageLine(const std::string& line) {
        auto pos = line.find(voltagePrefix);
        if (pos != std::string::npos) {
            std::string valStr = line.substr(pos + voltagePrefix.size());
            // valStr z.B. "51.42V\n"
            float voltage = 0.0f;
            if (sscanf(valStr.c_str(), "%fV", &voltage) == 1) {
                return voltage;
            }
        }
        return -1.0f; // Fehlerwert
    }

    void triggerLoad() {
        std::string cmd = "/Energiebox/Grid/grid -w " + std::to_string(config.grid.loadingCapacityWh) + " &";
        int ret = system(cmd.c_str());
        if (ret == -1) {
            std::cerr << "Fehler beim Starten des Grid-Ladevorgangs\n";
        } else {
            std::cout << "Grid-Ladevorgang gestartet (" << config.grid.loadingCapacityWh << " Wh)\n";
        }
    }

public:
    bool run() {
        if (!loadConfig()) {
            std::cerr << "Fehler: Konnte Config nicht laden: " << configPath << "\n";
            return false;
        }

        std::array<char, 256> buffer;
        FILE* pipe = popen(readallCmd.c_str(), "r");
        if (!pipe) {
            std::cerr << "Fehler: Konnte readall.py nicht ausführen\n";
            return false;
        }

        bool loadTriggered = false;
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            std::string line(buffer.data());
            if (line.find(voltagePrefix) == 0) {
                float voltage = parseVoltageLine(line);
                if (voltage < 0) {
                    std::cerr << "Warnung: Spannung konnte nicht geparst werden\n";
                    break;
                }
                std::cout << "Aktuelle Batteriespannung: " << voltage << " V\n";
                if (voltage < config.grid.battVoltageStartLoading) {
                    std::cout << "Niedrige Spannung erkannt, lade Grid...\n";
                    triggerLoad();
                    loadTriggered = true;
                } else {
                    std::cout << "Spannung ok, kein Laden notwendig.\n";
                }
                break;
            }
        }
        pclose(pipe);
        return loadTriggered;
    }
};

int main() {
    BatteryController controller;
    if (!controller.run()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
