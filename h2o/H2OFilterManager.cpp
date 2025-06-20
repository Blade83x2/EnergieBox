#include "H2OFilterManager.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <iomanip>
#include <cstdlib>
#include "iniparse.h"
#include "DebugLogger.h"
#include "config.h"

H2OFilterManager::H2OFilterManager(Configuration& configRef) : config(configRef) {}


void H2OFilterManager::startReinigung() {
    std::cout << "-> Spülvorgang beginnt...\n";
    sprintf(command, "12V %d 1 1", config.filter.ventil_pumpe);
    system(command);
    for (int i = 0; i < config.filter.filter_clean_time; ++i) {
        int bar_width = 53;
        float progress = static_cast<float>(i) / config.filter.filter_clean_time;
        int bar_length = progress * bar_width;
        std::cout << "\r -> [" << std::string(bar_length, '#') << std::string(bar_width - bar_length, ' ') << "] " << std::fixed << std::setprecision(2) << progress * 100 << "%";
        std::cout.flush();
        sleep(1);
    }
    sprintf(command, "12V %d 0 1", config.filter.ventil_pumpe);
    system(command);
    std::cout << "\n-> Spülung abgeschlossen.\n";
}

void H2OFilterManager::startFilterwechsel() {
    std::cout << "-> Filterwechsel\n";
    sprintf(command, "bash /Energiebox/h2o/setIni2.sh %d %d %d %d %d %d %d %f %d %d %f", config.filter.ventil_pumpe, config.filter.hex_generator, config.filter.filter_gpd, config.filter.filter_max_menge_filtern, config.filter.filter_verhaeltnis, config.filter.filter_clean_time, config.filter.filter_zeit_100ml, 0.00f,  config.tank.abwassertank_vorhanden, config.tank.abwassertank_groesse, config.tank.abwassertank_total_liter );
    system(command);
}

void H2OFilterManager::leereAbwassertank() {
    std::cout << "-> Abwasser-Zähler wird zurückgesetzt...\n";
    sprintf(command, "bash /Energiebox/h2o/setIni2.sh %d %d %d %d %d %d %d %f %d %d %f", config.filter.ventil_pumpe, config.filter.hex_generator, config.filter.filter_gpd, config.filter.filter_max_menge_filtern, config.filter.filter_verhaeltnis, config.filter.filter_clean_time, config.filter.filter_zeit_100ml, config.filter.filter_total_liter, config.tank.abwassertank_vorhanden, config.tank.abwassertank_groesse, 0.00f );
    system(command);
    std::cout << "-> Tank ist jetzt leer.\n";
}
