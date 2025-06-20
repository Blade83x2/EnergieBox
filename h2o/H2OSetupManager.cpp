#include "H2OSetupManager.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "iniparse.h"
#include "DebugLogger.h"
#include "config.h"
#include <limits>

extern Configuration config;
extern int handler(void* user, const char* section, const char* name, const char* value);

void H2OSetupManager::runSetup() {
    DebugLogger::setEnabled(true);
    if (ini_parse("/Energiebox/h2o/config.ini", handler, &config) < 0) {
        DebugLogger::log("Can't load '/Energiebox/h2o/config.ini", DebugLogger::ERROR);
        std::cerr << "Can't load '/Energiebox/h2o/config.ini'\n";
        return;
    }
    char startConfig;
    std::cout << "\n-> Einrichtung starten um neue Filteranlage zu konfigurieren? Y/N: ";
    std::cin >> startConfig;
    if (startConfig == 'Y' || startConfig == 'y' || startConfig == 'J' || startConfig == 'j') {
        int ventil_pumpe = 0;
        int hex_generator = 0;
        int filter_gpd = 0;
        int filter_max_menge_filtern = 0;
        int filter_verhaeltnis = 0;
        int filter_clean_time = 0;
        int abwassertank_vorhanden = 0;
        int abwassertank_groesse = 0;
        int filter_zeit_100ml = 0;
        float filter_total_liter = 0.f;
        float abwassertank_total_liter = 0.f;
        system("clear");
        while (true) {
            std::cout << "-> An welchem Relais von 12V (1-16) ist das Ventil oder die Wasserpumpe angeschlossen?: ";
            std::cin >> ventil_pumpe;
            if (std::cin.fail() || ventil_pumpe < 1 || ventil_pumpe > 16) {
                std::cin.clear(); // Fehlerstatus zurücksetzen
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ungültige Eingabe verwerfen
                std::cout << "\e[0;31m-> Ungültige Eingabe. Bitte eine ganze Zahl zwischen 1 und 16 eingeben.\n\e[0m";
            } else {
                // gültige Eingabe von ventil_pumpe
                while (true) {
                    std::cout << "-> An welchem Relais von 12V (1-16) ist das H³O² Frequenzgerät angeschlossen?: ";
                    std::cin >> hex_generator;
                    if (std::cin.fail() || hex_generator < 1 || hex_generator > 16 || hex_generator == ventil_pumpe) {
                        std::cin.clear(); // Fehlerstatus zurücksetzen
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ungültige Eingabe verwerfen
                        std::cout << "\e[0;31m-> Ungültige Eingabe. Zahl zwischen 1-16 eingeben. Darf nicht gleich " << ventil_pumpe << " sein!\n\e[0m";
                    } else {
                        // gültige Eingabe von hex_generator
                        while (true) {
                            std::cout << "-> Wieviel GPD hat die Filteranlage (50, 75, 100)?: ";
                            std::cin >> filter_gpd;
                            if (std::cin.fail() || (filter_gpd != 50 && filter_gpd != 75 && filter_gpd != 100) ) {
                                std::cin.clear(); // Fehlerstatus zurücksetzen
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ungültige Eingabe verwerfen
                                std::cout << "\e[0;31m-> Ungültige Eingabe. Mögliche Eingabe (50|75|100)!\n\e[0m";
                            } else {
                                // gültige Eingabe von filter_gpd
                                while (true) {
                                    std::cout << "-> Wieviel Liter sauberes Wasser darf maximal mit diesem Filter gefiltert werden?: ";
                                    std::cin >> filter_max_menge_filtern;
                                    if (std::cin.fail() || filter_max_menge_filtern < 1 || filter_max_menge_filtern > 2000 ) {
                                        std::cin.clear(); // Fehlerstatus zurücksetzen
                                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ungültige Eingabe verwerfen
                                        std::cout << "\e[0;31m-> Ungültige Eingabe. Zahl zwischen 1-2000 eingeben!\n\e[0m";
                                    } else {
                                        // gültige Eingabe von filter_max_menge_filtern
                                        while (true) {
                                            std::cout << "-> Wie ist das Verhältnis von gefiltertem Wasser zu Abwasser/Spülwasser? 1 zu?: ";
                                            std::cin >> filter_verhaeltnis;
                                            if (std::cin.fail() || filter_verhaeltnis < 1 || filter_verhaeltnis > 3 ) {
                                                std::cin.clear(); // Fehlerstatus zurücksetzen
                                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ungültige Eingabe verwerfen
                                                std::cout << "\e[0;31m-> Ungültige Eingabe. Zahl zwischen 1-3 eingeben!\n\e[0m";
                                            } else {
                                                // gültige Eingabe von filter_verhaeltnis
                                                while (true) {
                                                    std::cout << "-> Wieviele Sekunden sollen bei einer Spüllung gespült werden?: ";
                                                    std::cin >> filter_clean_time;
                                                    if (std::cin.fail() || filter_clean_time < 1 || filter_clean_time > 1800 ) {
                                                        std::cin.clear(); // Fehlerstatus zurücksetzen
                                                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ungültige Eingabe verwerfen
                                                        std::cout << "\e[0;31m-> Ungültige Eingabe. Zahl zwischen 1-1800 eingeben!\n\e[0m";
                                                    } else {
                                                        // gültige Eingabe von filter_clean_time
                                                        while (true) {
                                                            std::cout << "-> Ist ein Tank angeschlossen am Abwasser Schlauch (1=Ja, 0=Nein)?: ";
                                                            std::cin >> abwassertank_vorhanden;
                                                            if (std::cin.fail() || (abwassertank_vorhanden != 0 && abwassertank_vorhanden != 1) ) {
                                                                std::cin.clear(); // Fehlerstatus zurücksetzen
                                                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ungültige Eingabe verwerfen
                                                                std::cout << "\e[0;31m-> Ungültige Eingabe. Mögliche Zahl 1 oder 0 eingeben!\n\e[0m";
                                                            } else {
                                                                // gültige Eingabe von abwassertank_vorhanden
                                                                if (abwassertank_vorhanden == 1) {
                                                                    while (true) {
                                                                        std::cout << "-> Wieviel Fassungsvermögen in Liter hat der Tank?: ";
                                                                        std::cin >> abwassertank_groesse;
                                                                        if (std::cin.fail() || abwassertank_groesse < 1 || abwassertank_groesse > 10000 ) {
                                                                            std::cin.clear(); // Fehlerstatus zurücksetzen
                                                                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ungültige Eingabe verwerfen
                                                                            std::cout << "\e[0;31m-> Ungültige Eingabe. Zahl zwischen 1-10000 eingeben!\n\e[0m";
                                                                        } else {
                                                                            // gültige Eingabe von abwassertank_groesse
                                                                            break; 
                                                                        }
                                                                    } 
                                                                }
                                                                sprintf(command, "bash /Energiebox/h2o/setIni2.sh %d %d %d %d %d %d %d %f %d %d %f", ventil_pumpe, hex_generator, filter_gpd, filter_max_menge_filtern, filter_verhaeltnis, filter_clean_time, filter_zeit_100ml, filter_total_liter, abwassertank_vorhanden,abwassertank_groesse, abwassertank_total_liter  );
                                                                system(command);
                                                                //std::cout << "\n" << command << "\n\n";
                                                                break; 
                                                            }
                                                        }
                                                        break; 
                                                    }
                                                }
                                                break; 
                                            }
                                        }
                                        break; 
                                    }
                                }
                                break; 
                            }
                        }
                        break; 
                    }
                }
                break;
            }
        }
    }
}
