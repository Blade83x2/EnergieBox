#include "H2OFilterManager.h"
#include "H2OSetupManager.h"
#include "DebugLogger.h"
#include "config.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <iomanip>  // für std::setw, std::fixed, std::setprecision
#include <sstream>
#include <algorithm>
#include <vector>

H2OFilterManager::H2OFilterManager(Config& configRef) : config(configRef) {}

/**
 * Führt BASH Befehle in der Bash aus
 */
void H2OFilterManager::sendToBash(const std::string& command) {
    // beispiel: this->sendToBash(std::string("figlet -f mono12 ' Setup'"));
    int result = system(command.c_str());
    if (result == -1) {
        /* command erfolgreich */
    } else if (WIFEXITED(result) && WEXITSTATUS(result) != 0) {
        /* std::cerr << "Befehl beendet mit Fehlercode: " << WEXITSTATUS(result) << std::endl; */
    }
}

/**
 * Filtert das Wasser und Strukruriert es.
 */
void H2OFilterManager::filter(double menge, bool silentMode) {
    /**
     * TODO filter minimum und maximum eintragen. maximum in -setup einbauen
     */

    double abwassermenge = berechneAbwasserMenge(menge);
    int sekundenzeitfilter = berechneSekundenAusFiltermege(menge);
    double filter_total_liter = config.get<double>("filter/filter_total_liter");
    int filter_max_menge_filtern = config.get<int>("filter/filter_max_menge_filtern");
    int abwassertank_groesse = config.get<int>("tank/abwassertank_groesse");
    double abwassertank_total_liter = config.get<double>("tank/abwassertank_total_liter");
    int abwassertank_vorhanden = config.get<int>("tank/abwassertank_vorhanden");
    double restlicheMoeglicheMengeAbwasser = abwassertank_groesse - abwassertank_total_liter;
    double neuerTankFuellStand = abwassertank_total_liter + abwassermenge;
    int pin_filter = config.get<int>("filter/ventil_pumpe");
    int pin_hex_generator = config.get<int>("h3o2generator/hex_generator");
    int hexgeneratorLaufzeitSekunden = 100;
    // Daten überschtlich anzeigen
    if (!silentMode) {
        this->sendToBash(std::string("clear"));
        this->sendToBash(std::string("figlet -f mono12 ' H3O2'"));
        const int boxWidth = 57;
        const int labelWidth = 40;
        const int valueWidth = boxWidth - labelWidth - 5;
        if (abwassertank_vorhanden == 1) {
            printLine(boxWidth);
            std::cout << "  |" << centerText("TANK", boxWidth - 2) << "|" << std::endl;
            printLine(boxWidth);
            printDataRow(" Abwasser Menge:", floatToStr(abwassermenge) + " L", labelWidth + 1, valueWidth);
            printDataRow(" Gesamtgröße des Abwasser Tanks:", floatToStr(abwassertank_groesse) + " L", labelWidth + 3, valueWidth);
            printDataRow(" Aktueller Abwasser Tankfüllstand:", floatToStr(abwassertank_total_liter, 2) + " L", labelWidth + 2, valueWidth);
            printDataRow(" Restliche mögliche Abwasser Menge:", floatToStr(restlicheMoeglicheMengeAbwasser, 2) + " L", labelWidth + 2, valueWidth);
            printDataRow(" Neuer Abwasser Tankfüllstand:", floatToStr(neuerTankFuellStand, 2) + " L", labelWidth + 2, valueWidth);
            printLine(boxWidth);
        }
        printLine(boxWidth);
        std::cout << "  |" << centerText("FILTER", boxWidth - 2) << "|" << std::endl;
        printLine(boxWidth);
        printDataRow(" Filtermenge:", floatToStr(menge) + " L", labelWidth + 1, valueWidth);
        printDataRow(" Filterzeit (H:m:s):", formatSecondsToHMS(sekundenzeitfilter + hexgeneratorLaufzeitSekunden), labelWidth + 1, valueWidth);
        printDataRow(" Bislang gefiltertes Wasser:", floatToStr(filter_total_liter) + " L", labelWidth + 1, valueWidth);
        printDataRow(" Zulässige Filtermenge des Filters:", floatToStr(filter_max_menge_filtern, 0) + " L", labelWidth + 2, valueWidth);
        printLine(boxWidth);
        if (filter_total_liter >= filter_max_menge_filtern) {
            printDataRow("          >>> Warnung! Filter tauschen! <<<", "", 39, 10);
            printLine(boxWidth);
        }
    }
    // prüfen ob genug platz im abwassertank vorhanden ist
    bool tank_ok = false;
    if (abwassertank_vorhanden == 1) {
        if (restlicheMoeglicheMengeAbwasser >= abwassermenge) {
            tank_ok = true;
        } else {
            tank_ok = false;
            if (!silentMode) {
                printDataRow("        >>> Abbruch! Zuerst Tank leeren! <<<", "", 36, 9);
                printLine(57);
                std::cout << "\n";
            }
        }
    }
    if (tank_ok == true || abwassertank_vorhanden == 0) {
        // tank werte addieren falls aktiv
        if (abwassertank_vorhanden == 1) {
            this->addiereAbwasser(abwassermenge);
        }
        this->addiereGefiltertesWasser(menge);
        // filter einschalten
        this->sendToBash(std::string("12V " + std::to_string(pin_filter) + " 1 0"));
        // timer für filter auszuschalten
        this->sendToBash(std::string("12V " + std::to_string(pin_filter) + " 0 " + std::to_string(sekundenzeitfilter) + " &"));
        // timer für hexgenerator
        this->sendToBash(std::string("12V " + std::to_string(pin_hex_generator) + " 1 " + std::to_string(sekundenzeitfilter) + " &"));
        this->sendToBash(std::string("12V " + std::to_string(pin_hex_generator) + " 0 " + std::to_string(sekundenzeitfilter + 1) + " &"));
        // Progressbar anzeigen
        if (!silentMode) {
            int laufzeitSekunden = 0;
            while (true) {
                printProgress(std::string("  "), laufzeitSekunden, sekundenzeitfilter, 47);
                if (laufzeitSekunden == sekundenzeitfilter) {
                    break;
                }
                laufzeitSekunden++;
                sleep(1);
            }
            std::cout << "\n";
            printLine(57);
            std::cout << "  |" << centerText("HEXAGONALE STRUKTUR", 57 - 2) << "|" << std::endl;
            printLine(57);
            laufzeitSekunden = 0;
            while (true) {
                printProgress(std::string("  "), laufzeitSekunden, 100, 47);
                if (laufzeitSekunden == hexgeneratorLaufzeitSekunden) {
                    break;
                }
                laufzeitSekunden++;
                sleep(1);
            }
            std::cout << "\n";
            printLine(57);
            printLine(57);
            std::cout << "  |" << centerText("F E R T I G !", 57 - 2) << "|" << std::endl;
            printLine(57);
            std::cout << "\n\n";
        }
    }
}

/**
 * Berechnet Filterzeit aus gewünschter gefilterter Menge
 **/
int H2OFilterManager::berechneSekundenAusFiltermege(double wassermenge) {
    int multiplikator = (wassermenge * config.get<int>("filter/filter_zeit_100ml") * 10);
    return multiplikator;
}

/**
 * Zeigt eine Statistik der Einstellungen und Filter Nutzungsdaten an
 **/
void H2OFilterManager::printStatistik() {
    if (!silentMode) {
        this->sendToBash(std::string("clear"));
        this->sendToBash(std::string("figlet -f mono12 ' H3O2'"));
    }
    const int boxWidth = 57;
    const int labelWidth = 40;
    const int valueWidth = boxWidth - labelWidth - 5;  // 5 = "  | " + " |"
    printLine(boxWidth);
    std::cout << "  |" << centerText("STATISTIK", boxWidth - 2) << "|" << std::endl;
    printLine(boxWidth);
    // Filter Sektion
    printSectionTitle("Filter", boxWidth);
    printDataRow(" Bisherige Filtermenge:", floatToStr(config.get<float>("filter/filter_total_liter")) + " L", labelWidth + 1, valueWidth);
    printDataRow(" Faktor (Filtermenge->Abwassermenge):          1 ->", std::to_string(config.get<int>("filter/filter_verhaeltnis")), labelWidth, valueWidth - 10);
    printDataRow(" Filterlimit für Filtertausch:", std::to_string(config.get<int>("filter/filter_max_menge_filtern")) + " L", labelWidth + 2, valueWidth);
    float filterTotal = config.get<float>("filter/filter_total_liter");
    int filterLimit = config.get<int>("filter/filter_max_menge_filtern");
    printDataRow(" Restmenge bis Filtertausch:", floatToStr(filterLimit - filterTotal) + " L", labelWidth + 1, valueWidth);
    printDataRow(" Spülzeit in Sekunden:", std::to_string(config.get<int>("filter/filter_clean_time")), labelWidth + 2, valueWidth);
    printDataRow(" 12V ID von Ventil oder Pumpe:", std::to_string(config.get<int>("filter/ventil_pumpe")), labelWidth + 1, valueWidth);
    printLine(boxWidth);
    // H3O2 Sektion
    printSectionTitle("H³O² Generator", boxWidth + 2);
    printDataRow(" 12V ID von H³O² Generator:", std::to_string(config.get<int>("h3o2generator/hex_generator")), labelWidth + 3, valueWidth);
    printLine(boxWidth);
    // Abwasser Sektion
    printSectionTitle("Abwasser", boxWidth);
    int tankVorhanden = config.get<int>("tank/abwassertank_vorhanden");
    printDataRow(" Typ (Tank|Abfluss):", (tankVorhanden == 1 ? "Tank" : "Abwasser"), labelWidth + 1, valueWidth);
    printLine(boxWidth);
    // Tank Sektion
    printSectionTitle("Tank", boxWidth);
    printDataRow(" aktiv:", (tankVorhanden == 1 ? "Ja" : "Nein"), labelWidth + 1, valueWidth);
    if (tankVorhanden == 1) {
        printDataRow(" Fassungsvermögen:", floatToStr(config.get<float>("tank/abwassertank_groesse"), 0) + " L", labelWidth + 2, valueWidth);
        printDataRow(" Aktueller Füllstand:", floatToStr(config.get<float>("tank/abwassertank_total_liter")) + " L", labelWidth + 2, valueWidth);
    } else {
        printDataRow(" Fassungsvermögen:", "-", labelWidth + 2, valueWidth);
        printDataRow(" Aktueller Füllstand:", "-", labelWidth + 2, valueWidth);
    }
    printLine(boxWidth);
    std::cout << "\n" << std::endl;
}

/**
 * Ermittelt wieviel Abwasser produziert wird. Benötigt Filtermenge als Parameter
 */
double H2OFilterManager::berechneAbwasserMenge(double wassermenge) {
    int filter_verhaeltnis = config.get<int>("filter/filter_verhaeltnis");  // Verhältnis Gefiltertes Wasser zu Abwassermenge
    return wassermenge * filter_verhaeltnis;
}

/**
 * Ermittelt ob das Abwasser noch in den Abwassertank passt. Benötigt Abwassermenge als Parameter
 */
bool H2OFilterManager::berechneFreieAbwasserMengeImTank(double abwassermenge) {
    int abwassertank_groesse = config.get<int>("tank/abwassertank_groesse");
    double abwassertank_total_liter = config.get<double>("tank/abwassertank_total_liter");
    double free = abwassertank_groesse - abwassertank_total_liter;
    return (free > abwassermenge) ? true : false;
}

/**
 * Spüllt die Osmose Membrane durch
 **/
void H2OFilterManager::startReinigung() {
    if (!silentMode) {
        this->sendToBash(std::string("clear"));
        this->sendToBash(std::string("figlet -f mono12 ' H3O2'"));
    }
    int clean_time = config.get<int>("filter/filter_clean_time");
    std::cout << "\n  -> Soll die Membrane gereinigt werden? Benötigte Zeit: " + this->formatSecondsToHMS(clean_time) + "   J/N: ";
    char startClean;
    std::cin >> startClean;
    if (startClean == 'Y' || startClean == 'y' || startClean == 'J' || startClean == 'j') {
        std::cout << "  -> Wurde das Ventil an der Filteranlage auf Reinigung umgestellt? J/N: ";
        std::cin.clear();
        std::cin >> startClean;
        if (startClean == 'Y' || startClean == 'y' || startClean == 'J' || startClean == 'j') {
            std::cin.clear();  // Fehlerstatus zurücksetzen
            int ventil = config.get<int>("filter/ventil_pumpe");
            int tankActive = config.get<int>("tank/abwassertank_vorhanden");
            bool doIt = false;
            if (tankActive == 1) {
                // berechnen wieviel abwasser produziert wird
                if (this->berechneFreieAbwasserMengeImTank(this->berechneAbwasserMenge(this->berechneLiterMengeAusZeit(clean_time)))) {
                    doIt = true;
                    std::cout << "  -> Die Anlage wird gereinigt..";
                } else {
                    doIt = false;
                    std::cout << "  \e[0;31m-> Abbruch! Bitte zuerst Tank leeren!\e[0m";
                }
            } else {
                doIt = true;
            }
            if (doIt) {
                std::string cmd = "12V " + std::to_string(ventil) + " 1 0";
                system(cmd.c_str());
                cmd = "12V " + std::to_string(ventil) + " 0 " + std::to_string(clean_time) + " &";
                system(cmd.c_str());
                if (tankActive == 1) {
                    this->addiereAbwasser(this->berechneAbwasserMenge(this->berechneLiterMengeAusZeit(clean_time)));
                }
                for (int i = 0; i <= clean_time; ++i) {
                    H2OFilterManager::printProgress(std::string("  -> "), i, clean_time, 59);
                    sleep(1);
                }
                std::cout << "\n  -> Reinigung abgeschlossen!";
            }
            std::cout << "\n  -> Ventil wieder auf normal stellen!";
        }
    }
    std::cout << "\n" << std::endl;
}

/**
 * Wechselt die Filter Kartuschen und stellt Werte in config.ini zurück
 **/
void H2OFilterManager::startFilterwechsel() {
    if (!silentMode) {
        this->sendToBash(std::string("clear"));
        this->sendToBash(std::string("figlet -f mono12 ' H3O2'"));
    }
    std::cout << "\n  -> Wurde der Filter getauscht? J/N: ";
    char startChange;
    std::cin >> startChange;
    if (startChange == 'Y' || startChange == 'y' || startChange == 'J' || startChange == 'j') {
        config.set<double>("filter/filter_total_liter", 0.000000);
        std::cout << "  -> Filter Aufzeichnungen wurden zurück gestellt!";
    }
    std::cout << "\n" << std::endl;
}

/**
 * Stellt bei aktiviertem Tank den Füllstand zurück
 */
void H2OFilterManager::leereAbwassertank() {
    if (!silentMode) {
        this->sendToBash(std::string("clear"));
        this->sendToBash(std::string("figlet -f mono12 ' H3O2'"));
    }
    int tankActive = config.get<int>("tank/abwassertank_vorhanden");
    if (tankActive == 1) {
        std::cout << "\n  -> Soll der Abwassertank gelert werden? J/N: ";
        char startEmpty;
        std::cin >> startEmpty;
        if (startEmpty == 'Y' || startEmpty == 'y' || startEmpty == 'J' || startEmpty == 'j') {
            config.set<double>("tank/abwassertank_total_liter", 0.000000);
            std::cout << "  -> Abwassertank Aufzeichnungen wurden zurück gestellt!";
        }
    } else {
        std::cout << "  \e[0;31m-> Der Abwassertank ist nicht konfiguriert!\e[0m";
    }
    std::cout << "\n" << std::endl;
}

/**
 * Berechnet gefilterte Milliliter. Benötigt Zeit in Sekunden
 **/
double H2OFilterManager::berechneLiterMengeAusZeit(double laufzeit) {
    double mlReferenz = 100;
    double zeitReferenz = config.get<int>("filter/filter_zeit_100ml");
    return ((mlReferenz / zeitReferenz) * laufzeit) / 1000;
}

/**
 * Addiert die Menge des neu gefilterten Wasser zu filter/filter_total_liter in config.ini
 */
void H2OFilterManager::addiereGefiltertesWasser(double filterMengeInLiter) {
    double filter_total_liter = config.get<double>("filter/filter_total_liter");
    filter_total_liter += filterMengeInLiter;
    config.set<double>("filter/filter_total_liter", filter_total_liter);
}

/**
 * Addiert abwasserMengeInLiter zu tank/abwassertank_total_liter in config.ini
 */
void H2OFilterManager::addiereAbwasser(double abwasserMengeInLiter) {
    double abwassertank_total_liter = config.get<double>("tank/abwassertank_total_liter");
    abwassertank_total_liter += abwasserMengeInLiter;
    config.set<double>("tank/abwassertank_total_liter", abwassertank_total_liter);
}

/**
 * Gibt aus Sekunden eine formatierte Zeitanzeige aus
 **/
std::string H2OFilterManager::formatSecondsToHMS(int seconds) {
    /*  std::cout << formatSecondsToHMS(732) << std::endl;    ----> Ausgabe: 00:12:12  */
    int h = seconds / 3600;
    int m = (seconds % 3600) / 60;
    int s = seconds % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << h << ":" << std::setw(2) << std::setfill('0') << m << ":" << std::setw(2) << std::setfill('0') << s;
    return oss.str();
}

/* printProgress("-> ", i, maxValue, 53); */
void H2OFilterManager::printProgress(const std::string& prefix, size_t count, size_t max, const int bar_width) {
    float progress = static_cast<float>(count) / max;
    int bar_length = static_cast<int>(progress * bar_width);
    std::cout << "\r" << std::string(prefix) << "[" << std::string(bar_length, '#') << std::string(bar_width - bar_length, ' ') << "] " << std::fixed << std::setprecision(2)
              << progress * 100 << "%";
    std::cout.flush();
}

std::string H2OFilterManager::centerText(const std::string& text, int width) {
    int len = static_cast<int>(text.length());
    if (len >= width) return text;
    int leftPadding = (width - len) / 2;
    int rightPadding = width - len - leftPadding;
    return std::string(leftPadding, ' ') + text + std::string(rightPadding, ' ');
}

void H2OFilterManager::printLine(int width) {
    std::cout << "  +" << std::string(width - 2, '-') << "+" << std::endl;
}

void H2OFilterManager::printSectionTitle(const std::string& title, int width) {
    std::cout << "  | " << std::left << std::setw(width - 4) << title << " |" << std::endl;
}

void H2OFilterManager::printDataRow(const std::string& label, const std::string& value, int labelWidth, int valueWidth) {
    std::cout << "  | " << std::left << std::setw(labelWidth) << label << std::right << std::setw(valueWidth) << value << " |" << std::endl;
}

// Format float mit n Nachkommastellen als String
std::string H2OFilterManager::floatToStr(float val, int decimals) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << val;
    return oss.str();
}