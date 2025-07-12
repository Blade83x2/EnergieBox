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

/* printProgress(i, maxValue, 53); */
void H2OFilterManager::printProgress(size_t count, size_t max, const int bar_width) {
    float progress = static_cast<float>(count) / max;
    int bar_length = static_cast<int>(progress * bar_width);
    std::cout << "\r -> [" << std::string(bar_length, '#') << std::string(bar_width - bar_length, ' ') << "] " << std::fixed << std::setprecision(2) << progress * 100 << "%";
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

// Haupt-Ausgabefunktion
void H2OFilterManager::printStatistik() {
    const int boxWidth = 57;
    const int labelWidth = 40;
    const int valueWidth = boxWidth - labelWidth - 5;  // 5 = "  | " + " |"
    printLine(boxWidth);
    std::cout << "  |" << centerText("STATISTIK", boxWidth - 2) << "|" << std::endl;
    printLine(boxWidth);
    // Filter Sektion
    printSectionTitle("Filter", boxWidth);
    printDataRow(" Leistung:", std::to_string(config.get<int>("filter/filter_gpd")) + " GPD", labelWidth + 1, valueWidth);
    printDataRow(" Faktor (Filtermenge->Abwassermenge):          1 ->", std::to_string(config.get<int>("filter/filter_verhaeltnis")), labelWidth, valueWidth - 10);
    printDataRow(" Bisherige Filtermenge:", floatToStr(config.get<float>("filter/filter_total_liter")) + " L", labelWidth + 1, valueWidth);
    printDataRow(" Filterlimit für Filtertausch:", std::to_string(config.get<int>("filter/filter_max_menge_filtern")) + " L", labelWidth + 2, valueWidth);
    float filterTotal = config.get<float>("filter/filter_total_liter");
    int filterLimit = config.get<int>("filter/filter_max_menge_filtern");
    printDataRow(" Restmenge bis Filtertausch:", floatToStr(filterLimit - filterTotal) + " L", labelWidth + 1, valueWidth);
    printDataRow(" Spülzeit in Sekunden:", std::to_string(config.get<int>("filter/filter_clean_time")), labelWidth + 2, valueWidth);
    printDataRow(" 12V ID von Ventil oder Pumpe:", std::to_string(config.get<int>("filter/ventil_pumpe")), labelWidth + 1, valueWidth);
    printDataRow(" 12V ID von H³O² Generator:", std::to_string(config.get<int>("filter/hex_generator")), labelWidth + 3, valueWidth);
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

void H2OFilterManager::startReinigung() {
    std::cout << "-> Spülvorgang beginnt...\n";
    int ventil = config.get<int>("filter/ventil_pumpe");
    int clean_time = config.get<int>("filter/filter_clean_time");
    std::string cmd1 = "12V " + std::to_string(ventil) + " 1 0";
    system(cmd1.c_str());
    std::string cmd2 = "12V " + std::to_string(ventil) + " 0 " + std::to_string(clean_time) + " &";
    system(cmd2.c_str());
    for (int i = 0; i <= config.get<int>("filter/filter_clean_time"); ++i) {
        H2OFilterManager::printProgress(i, config.get<int>("filter/filter_clean_time"), 53);
        sleep(1);
    }
    std::cout << "\n-> Spülung abgeschlossen.\n";
}

void H2OFilterManager::startFilterwechsel() {
    printIfNotSilent("-> Filterwechsel\n");
    std::string command = "bash /Energiebox/h2o/setIni2.sh " + std::to_string(config.get<int>("filter/ventil_pumpe")) + " " +
                          std::to_string(config.get<int>("filter/hex_generator")) + " " + std::to_string(config.get<int>("filter/filter_gpd")) + " " +
                          std::to_string(config.get<int>("filter/filter_max_menge_filtern")) + " " + std::to_string(config.get<int>("filter/filter_verhaeltnis")) + " " +
                          std::to_string(config.get<int>("filter/filter_clean_time")) + " " + std::to_string(config.get<int>("filter/filter_zeit_100ml")) + " " +
                          "0.00 " +  // filter_total_liter wird beim Wechsel auf 0 gesetzt
                          std::to_string(config.get<int>("tank/abwassertank_vorhanden")) + " " + std::to_string(config.get<int>("tank/abwassertank_groesse")) + " " +
                          std::to_string(config.get<float>("tank/abwassertank_total_liter"));
    system(command.c_str());
    printIfNotSilent("-> Filter wurde gewechselt.\n");
}

void H2OFilterManager::leereAbwassertank() {
    printIfNotSilent("-> Abwasser-Zähler wird zurückgesetzt...\n");
    std::string command = "bash /Energiebox/h2o/setIni2.sh " + std::to_string(config.get<int>("filter/ventil_pumpe")) + " " +
                          std::to_string(config.get<int>("filter/hex_generator")) + " " + std::to_string(config.get<int>("filter/filter_gpd")) + " " +
                          std::to_string(config.get<int>("filter/filter_max_menge_filtern")) + " " + std::to_string(config.get<int>("filter/filter_verhaeltnis")) + " " +
                          std::to_string(config.get<int>("filter/filter_clean_time")) + " " + std::to_string(config.get<int>("filter/filter_zeit_100ml")) + " " +
                          std::to_string(config.get<float>("filter/filter_total_liter")) + " " + std::to_string(config.get<int>("tank/abwassertank_vorhanden")) + " " +
                          std::to_string(config.get<int>("tank/abwassertank_groesse")) + " " + "0.00";  // ← Zähler auf 0 zurücksetzen
    system(command.c_str());
    printIfNotSilent("-> Tank ist jetzt leer.\n");
}

// Konvertiert GPD (Gallons per Day) in Liter pro Sekunde
double H2OFilterManager::gpdToLitersPerSecond(double gpd) {
    /*
     * Beispiel für Sekundenproduktion
     *  double gpd = 50.0;
     *  double lps = gpdToLitersPerSecond(gpd);
     *  std::cout << gpd << " GPD entsprechen " << lps << " Litern pro Sekunde." << std::endl;
     *
     */
    const double gallonsToLiters = 3.78541;
    const double secondsPerDay = 86400.0;
    return (gpd * gallonsToLiters) / secondsPerDay;
}

// Berechnet die benötigte Filterzeit in Sekunden für eine gegebene Literzahl
int H2OFilterManager::filterDauerInSekunden(double literZiel, double gpd) {
    /*
     * double gpd = 50.0;           // Filterleistung
     * double literZiel = 10.0;     // gewünschte Literanzahl
     * int sekunden = filterDauerInSekunden(literZiel, gpd);
     * std::cout << std::fixed << std::setprecision(2);
     * std::cout << "Um " << literZiel << " Liter mit " << gpd << " GPD zu filtern,\n"  << "müsste der Filter etwa " << sekunden << " Sekunden laufen." << std::endl;
     */
    double literProSekunde = this->gpdToLitersPerSecond(gpd);
    if (literProSekunde <= 0.0) {
        std::cerr << "Fehler: GPD darf nicht null oder negativ sein." << std::endl;
        return -1;
    }
    return static_cast<int>(literZiel / literProSekunde);
}

// Gibt aus Sekunden eine formatierte Zeitanzeige aus
std::string H2OFilterManager::formatSecondsToHMS(int seconds) {
    /*
     * std::cout << formatSecondsToHMS(732) << std::endl;    ----> Ausgabe: 00:12:12
     */
    int h = seconds / 3600;
    int m = (seconds % 3600) / 60;
    int s = seconds % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << h << ":" << std::setw(2) << std::setfill('0') << m << ":" << std::setw(2) << std::setfill('0') << s;
    return oss.str();
}
