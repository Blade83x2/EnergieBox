#include "H2OSetupManager.h"
#include "config.h"
#include "H2OFilterManager.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "DebugLogger.h"
#include <limits>
#include <string>
#include <chrono>  // zeitmessung

/**
 * TODO
 * Maximales Filterlimit bei -setup einbauen
 * Minimales FilterLimit im Code definieren
 *
 **/

/**
 * Konstruktor
 */
H2OSetupManager::H2OSetupManager(Config& configRef) : config(configRef) {}

/**
 * Führt BASH Befehle in der Bash aus
 */
void H2OSetupManager::sendToBash(const std::string& command) {
    // beispiel: this->sendToBash(std::string("figlet -f mono12 ' Setup'"));
    int result = system(command.c_str());
    if (result == -1) {
        /* command erfolgreich */
    } else if (WIFEXITED(result) && WEXITSTATUS(result) != 0) {
        /* std::cerr << "Befehl beendet mit Fehlercode: " << WEXITSTATUS(result) << std::endl; */
    }
}

/**
 * Ermittelt die Laufzeit des Filters für einen Liter gefiltertes Wasser
 */
void H2OSetupManager::runSpeedtest() {
    this->sendToBash("clear");
    this->sendToBash("figlet -f mono12 ' H3O2'");
    std::cout << "\n  Jeder Filter hat eine andere Filtergeschwindigkeit! Damit das Programm weiß,";
    std::cout << "\n  wann es den Filter abschalten soll, benötigt es die genaue Zeit zur Filterung.";
    std::cout << "\n  -> Möchtest du den Filter-Speedtest starten um die Zeit zu erfassen? J/N: ";
    char input;
    std::cin >> input;
    if (input != 'j' && input != 'J' && input != 'y' && input != 'Y') {
        std::cout << "  \e[0;31m-> Abbruch\n\n\e[0m";
        return;
    }
    std::cout << "  -> Bereit? Bitte miss jetzt 1 Liter gefiltertes Wasser manuell.";
    std::cout << "\n  -> Drücke ENTER, sobald du bereit bist, den Filter zu starten...";
    std::cin.ignore();  // Schluckt evtl. Restzeichen aus vorheriger Eingabe
    std::cin.get();     // Warten auf ENTER
    int ventil = config.get<int>("filter/ventil_pumpe");
    std::cout << "  -> Filter wird jetzt eingeschaltet...\n";
    std::string cmd = "12V " + std::to_string(ventil) + " 1 0";
    system(cmd.c_str());
    auto start = std::chrono::steady_clock::now();
    std::cout << "  -> Drücke ENTER, sobald 1 Liter durchgefiltert wurde...";
    std::cin.get();  // warten auf ENTER
    auto end = std::chrono::steady_clock::now();
    cmd = "12V " + std::to_string(ventil) + " 0 0";
    system(cmd.c_str());
    int filter_zeit_100ml = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "  -> Der Filter hat " << std::to_string(filter_zeit_100ml) << " Sekunden für 1 Liter benötigt.\n\n";
    config.set<int>("filter/filter_zeit_100ml", filter_zeit_100ml / 10);
}

/**
 * Fragt den Benutzer nach Filterspezifischen Daten ab und speichert die Konfiguration
 */
void H2OSetupManager::runSetup() {
    this->sendToBash(std::string("clear"));
    this->sendToBash(std::string("figlet -f mono12 ' H3O2'"));
    std::cout << "\n  -> Einrichtung starten um neue Filteranlage zu konfigurieren? J/N: ";
    char startConfig;
    std::cin >> startConfig;
    if (startConfig == 'Y' || startConfig == 'y' || startConfig == 'J' || startConfig == 'j') {
        int ventil_pumpe = 0;
        int hex_generator = 0;
        int filter_max_menge_filtern = 0;
        int filter_verhaeltnis = 0;
        int filter_clean_time = 0;
        int abwassertank_vorhanden = 0;
        int abwassertank_groesse = 0;
        while (true) {
            std::cout << "  -> An welchem Relais von 12V (1-16) ist das Ventil oder die Wasserpumpe angeschlossen?: ";
            std::cin >> ventil_pumpe;
            if (std::cin.fail() || ventil_pumpe < 1 || ventil_pumpe > 16) {
                std::cin.clear();                                                    // Fehlerstatus zurücksetzen
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // ungültige Eingabe verwerfen
                std::cout << "  \e[0;31m-> Ungültige Eingabe. Bitte eine ganze Zahl zwischen 1 und 16 eingeben.\n\e[0m";
            } else {
                config.set<int>("filter/ventil_pumpe", ventil_pumpe);
                // gültige Eingabe von ventil_pumpe
                while (true) {
                    std::cout << "  -> An welchem Relais von 12V (1-16) ist das H³O² Frequenzgerät angeschlossen?: ";
                    std::cin >> hex_generator;
                    if (std::cin.fail() || hex_generator < 1 || hex_generator > 16 || hex_generator == ventil_pumpe) {
                        std::cin.clear();                                                    // Fehlerstatus zurücksetzen
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // ungültige Eingabe verwerfen
                        std::cout << "  \e[0;31m-> Ungültige Eingabe. Zahl zwischen 1-16 eingeben. Darf nicht gleich " << ventil_pumpe << " sein!\n\e[0m";
                    } else {
                        config.set<int>("h3o2generator/hex_generator", hex_generator);
                        // gültige Eingabe von hex_generator
                        while (true) {
                            std::cout << "  -> Wieviel Liter sauberes Wasser sollte maximal mit diesem Filter gefiltert werden?: ";
                            std::cin >> filter_max_menge_filtern;
                            if (std::cin.fail() || filter_max_menge_filtern < 1 || filter_max_menge_filtern > 5000) {
                                std::cin.clear();                                                    // Fehlerstatus zurücksetzen
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // ungültige Eingabe verwerfen
                                std::cout << "  \e[0;31m-> Ungültige Eingabe. Zahl zwischen 1-5000 eingeben!\n\e[0m";
                            } else {
                                config.set<int>("filter/filter_max_menge_filtern", filter_max_menge_filtern);
                                // gültige Eingabe von filter_max_menge_filtern
                                while (true) {
                                    std::cout << "  -> Wie ist das Verhältnis von gefiltertem Wasser zu Abwasser/Spülwasser? 1 zu?: ";
                                    std::cin >> filter_verhaeltnis;
                                    if (std::cin.fail() || filter_verhaeltnis < 1 || filter_verhaeltnis > 3) {
                                        std::cin.clear();                                                    // Fehlerstatus zurücksetzen
                                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // ungültige Eingabe verwerfen
                                        std::cout << "  \e[0;31m-> Ungültige Eingabe. Zahl zwischen 1-3 eingeben!\n\e[0m";
                                    } else {
                                        config.set<int>("filter/filter_verhaeltnis", filter_verhaeltnis);
                                        // gültige Eingabe von filter_verhaeltnis
                                        while (true) {
                                            std::cout << "  -> Wieviele Sekunden sollen bei einer Spülung gespült werden?: ";
                                            std::cin >> filter_clean_time;
                                            if (std::cin.fail() || filter_clean_time < 1 || filter_clean_time > 1800) {
                                                std::cin.clear();                                                    // Fehlerstatus zurücksetzen
                                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // ungültige Eingabe verwerfen
                                                std::cout << "  \e[0;31m-> Ungültige Eingabe. Zahl zwischen 1-1800 eingeben!\n\e[0m";
                                            } else {
                                                config.set<int>("filter/filter_clean_time", filter_clean_time);
                                                config.set<int>("filter/filter_zeit_100ml", 52);
                                                config.set<double>("filter/filter_total_liter", 0.000000);
                                                // gültige Eingabe von filter_clean_time
                                                while (true) {
                                                    std::cout << "  -> Ist ein Tank angeschlossen am Abwasser Schlauch (1=Ja, 0=Nein)?: ";
                                                    std::cin >> abwassertank_vorhanden;
                                                    if (std::cin.fail() || (abwassertank_vorhanden != 0 && abwassertank_vorhanden != 1)) {
                                                        std::cin.clear();                                                    // Fehlerstatus zurücksetzen
                                                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // ungültige Eingabe verwerfen
                                                        std::cout << "  \e[0;31m-> Ungültige Eingabe. Mögliche Zahl 1 oder 0 eingeben!\n\e[0m";
                                                    } else {
                                                        config.set<int>("tank/abwassertank_vorhanden", abwassertank_vorhanden);
                                                        // gültige Eingabe von abwassertank_vorhanden
                                                        if (abwassertank_vorhanden == 1) {
                                                            while (true) {
                                                                std::cout << "  -> Wieviel Fassungsvermögen in Liter hat der Tank?: ";
                                                                std::cin >> abwassertank_groesse;
                                                                if (std::cin.fail() || abwassertank_groesse < 1 || abwassertank_groesse > 10000) {
                                                                    std::cin.clear();                                                    // Fehlerstatus zurücksetzen
                                                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // ungültige Eingabe verwerfen
                                                                    std::cout << "  \e[0;31m-> Ungültige Eingabe. Zahl zwischen 1-10000 eingeben!\n\e[0m";
                                                                } else {
                                                                    // gültige Eingabe von abwassertank_groesse
                                                                    config.set<int>("tank/abwassertank_groesse", abwassertank_groesse);
                                                                    break;
                                                                }
                                                            }
                                                        } else {
                                                            config.set<int>("tank/abwassertank_groesse", 0);
                                                        }
                                                        config.set<double>("tank/abwassertank_total_liter", 0.000000);
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
        std::cout << "  -> Einrichtung beendet! Filterzeit konfigurieren? J/N: ";
        char startSpeedtest;
        std::cin >> startSpeedtest;
        if (startSpeedtest == 'Y' || startSpeedtest == 'y' || startSpeedtest == 'J' || startSpeedtest == 'j') {
            this->sendToBash(std::string("h3o2 -speedtest"));
        }
    }
    std::cout << "\n";
}
