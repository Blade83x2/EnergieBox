// H2O Osmose Filter Controller
// Vendor: Johannes Krämer
// Version: 1.6 (C++ Rewrite)
// Date: 19.06.2025

//  sudo g++ -o h2o_test h2o_test.cpp H2OSetupManager.cpp DebugLogger.cpp H2OFilterManager.cpp iniparse.o mymcp23017.o -std=c++17 -lwiringPi


#include <iostream>
#include "config.h"
#include "H2OSetupManager.h"
#include "H2OFilterManager.h"
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <cmath>
#include <limits>
#include "iniparse.h"
#include "mymcp23017.h"
#include "DebugLogger.h"

using namespace std;

Configuration config;

char command[256];

void replace_char(string& s, char find, char replace) {
    for (char& c : s) if (c == find) c = replace;
}

void print_progress(size_t count, size_t max) {
    const int bar_width = 53;
    float progress = static_cast<float>(count) / max;
    int bar_length = progress * bar_width;
    cout << "\r -> [" << string(bar_length, '#') << string(bar_width - bar_length, ' ') << "] " << fixed << setprecision(2) << progress * 100 << "%";
    cout.flush();
}

void print_progress2(size_t count, size_t max) {
    const int bar_width = 38;
    float progress = static_cast<float>(count) / max;
    int bar_length = progress * bar_width;
    cout << "\r -> [" << string(bar_length, '#') << string(bar_width - bar_length, ' ') << "] " << fixed << setprecision(2) << progress * 100 << "%";
    cout.flush();
}

int handler(void* user, const char* section, const char* name, const char* value) {
    Configuration* pconfig = (Configuration*)user;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("filter", "ventil_pumpe")) pconfig->filter.ventil_pumpe = atoi(value);
    else if (MATCH("filter", "hex_generator")) pconfig->filter.hex_generator = atoi(value);
    else if (MATCH("filter", "filter_gpd")) pconfig->filter.filter_gpd = atoi(value);
    else if (MATCH("filter", "filter_max_menge_filtern")) pconfig->filter.filter_max_menge_filtern = atoi(value);
    else if (MATCH("filter", "filter_verhaeltnis")) pconfig->filter.filter_verhaeltnis = atoi(value);
    else if (MATCH("filter", "filter_clean_time")) pconfig->filter.filter_clean_time = atoi(value);
    else if (MATCH("filter", "filter_zeit_100ml")) pconfig->filter.filter_zeit_100ml = atoi(value);
    else if (MATCH("filter", "filter_total_liter")) pconfig->filter.filter_total_liter = atof(value);
    else if (MATCH("tank", "abwassertank_vorhanden")) pconfig->tank.abwassertank_vorhanden = atoi(value);
    else if (MATCH("tank", "abwassertank_groesse")) pconfig->tank.abwassertank_groesse = atoi(value);
    else if (MATCH("tank", "abwassertank_total_liter")) pconfig->tank.abwassertank_total_liter = atof(value);
    else return 0;
    return 1;
   
}

void showLogo() {
    cout << R"(
   _   _ ____   ___
  | | | |___ \ / _ \
  | |_| | __) | | | | 
  |  _  |/ __/| |_| | 
  |_| |_|_____|\___/  
)" << endl;
}

void showHelp() {
    cout << R"(
 Tool für automatische Filtersteuerung. Anwendungsbeispiel:

  h2o 0.2     [Produziert 0,2 Liter gefiltertes Wasser]
  h2o -clean  [Spült die Membrane]
  h2o -change [Filter austauschen und Zähler auf 0 setzen]
  h2o -empty  [Setzt Tank für Abwasser auf 0 Liter]
  h2o -stats  [Zeigt Statistik über Filter und Tank]
  h2o -setup  [Filter konfigurieren]
)" << endl;
}


int main(int argc, char* argv[]) {
    DebugLogger::setEnabled(true);
    // Konfiguration laden
    if (ini_parse("/Energiebox/h2o/config2.ini", handler, &config) < 0) {
        DebugLogger::log("Can't load '/Energiebox/h2o/config.ini", DebugLogger::ERROR);
        cerr << "Can't load '/Energiebox/h2o/config.ini'\n";
        return 1;
    }
    // Sicherstellen, dass Pumpe aus ist
    sprintf(command, "12V %d 0 1", config.filter.ventil_pumpe);
    system(command);
    
    showLogo();

    if (argc == 1) {
        showHelp();
        return 0;
    }

    string param = argv[1];
    if (param == "-clean") {
        H2OFilterManager filter(config);
        filter.startReinigung();

    }
    else if (param == "-change") {
        H2OFilterManager filter(config);
        filter.startFilterwechsel();
    }
    else if (param == "-empty") {
        H2OFilterManager filter(config);
        filter.leereAbwassertank();
    }
    else if (param == "-setup") {
        H2OSetupManager setup(config);
        setup.runSetup();
    }
    else if (param == "-stats") {
        H2OFilterManager filter(config);
        filter.printStatistik();
    }
    else {
        // Versuchen, Parameter als Zahl (Filtermenge) zu interpretieren
        replace_char(param, ',', '.'); // Komma zu Punkt für atof/stof
        try {
            float filterMenge = stof(param);

            if (filterMenge < 0.1f) {
                cout << "\n\e[0;31m Die minimale Wassermenge zum filtern muss mindestens 0.1 Liter betragen!\n\n\e[0m";
                return 0;
            }

            cout << "\n\n -> Filtermenge:\t\t\t" << fixed << setprecision(1) << filterMenge << " Liter\n";
            cout << " -> Filterleistung:\t\t\t   " << config.filter.filter_gpd << " GPD\n";

            float filterLaufzeit = (filterMenge / 0.1f) * config.filter.filter_zeit_100ml;
            int filterLaufzeitInt = static_cast<int>(filterLaufzeit);

            int s = filterLaufzeitInt;
            int stunden = s / 3600;
            s %= 3600;
            int minuten = s / 60;
            s %= 60;
            int sekunden = s;

            float abwasserMenge = filterMenge * config.filter.filter_verhaeltnis;

            cout << " -> Berechnete Abwasser Menge:\t\t" << fixed << setprecision(1) << abwasserMenge << " Liter\n";
            cout << " -> Berechnete Filter Zeit:\t\t" << fixed << setprecision(0) << filterLaufzeit << " Sek.\n";
            cout << " -> Bislang gefiltertes Wasser:\t\t" << fixed << setprecision(1) << config.filter.filter_total_liter << " Liter\n";
            cout << " -> Max. Empfohlene Filtermenge:\t" << fixed << setprecision(0) << config.filter.filter_max_menge_filtern << " Liter\n";
            cout << " -> Restliche mögliche Abwasser Menge:\t" << fixed << setprecision(1) << (config.tank.abwassertank_total_liter - config.tank.abwassertank_total_liter) << " Liter\n";
            cout << " -> Aktueller Abwasser Tankfüllstand:\t" << fixed << setprecision(1) << config.tank.abwassertank_total_liter << " Liter\n";
            cout << " -> Gesamtgröße des Abwasser Tanks:\t" << fixed << setprecision(1) << config.tank.abwassertank_total_liter << " Liter\n";

            if (config.filter.filter_total_liter >= config.filter.filter_max_menge_filtern) {
                cout << "\e[0;31m -> Die maximal empfohlene Nutzungsmenge des Filters\n    von " << config.filter.filter_total_liter << " Litern ist erreicht / überschritten.\n    Der Filter sollte gewechselt werden!\e[0m\n";
            }

            if ((abwasserMenge + config.tank.abwassertank_total_liter) <= config.tank.abwassertank_groesse) {
                // Konfiguration aktualisieren
                /*
    sprintf(command, "bash /Energiebox/h2o/setIni2.sh %d %d %d %d %d %d %d %f %d %d %f", config.filter.ventil_pumpe, config.filter.hex_generator, config.filter.filter_gpd, config.filter.filter_max_menge_filtern, config.filter.filter_verhaeltnis, config.filter.filter_clean_time, config.filter.filter_zeit_100ml, 0.00f,  config.tank.abwassertank_vorhanden, config.tank.abwassertank_groesse, config.tank.abwassertank_total_liter );*/
                /*
                snprintf(command, sizeof(command),
                         "sh /Energiebox/h2o/setIni.sh %f %f %d %d %f %d %d %f %d",
                         (config.tank.abwassertank_total_liter + abwasserMenge),
                         (config.filter.filter_total_liter + filterMenge),
                         config.filter.ventil_pumpe,
                         config.filter.filter_gpd,
                         config.filter.filter_max_menge_filtern,
                         config.filter.filter_zeit_100ml,
                         config.filter.filter_verhaeltnis,
                         config.tank.abwassertank_total_liter,
                         config.filter.filter_clean_time);*/
               // system(command);

                // Pumpe einschalten
                snprintf(command, sizeof(command), "12V %d 1 1", config.filter.ventil_pumpe);
                system(command);

                cout << "\n -> Wasser wird gefiltert. Benötigte Zeit: "
                     << setfill('0') << setw(2) << stunden << ":"
                     << setw(2) << minuten << ":"
                     << setw(2) << sekunden << "\n";

                int laufzeitSekunden = 0;
                while (laufzeitSekunden < filterLaufzeitInt) {
                    print_progress2(laufzeitSekunden, filterLaufzeitInt);
                    sleep(1);
                    laufzeitSekunden++;
                }
                print_progress2(filterLaufzeitInt, filterLaufzeitInt);

                // Pumpe ausschalten
                snprintf(command, sizeof(command), "12V %d 0 1", config.filter.ventil_pumpe);
                system(command);

                cout << "\n -> FERTIG!\n";
            }
            else {
                cout << "\e[0;31m -> Die benötigte Abwasser Menge ist zuviel.\n    Zuerst Abwasser Tank entleeren mit h2o -empty!\n\n\e[0m";
            }
        }
        catch (const invalid_argument&) {
            cout << "\n\e[0;31m Ungültiger Parameter!\n\n\e[0m";
            showHelp();
            return 1;
        }
    }

    return 0;
}
