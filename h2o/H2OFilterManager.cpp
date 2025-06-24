#include "H2OFilterManager.h"
#include "H2OSetupManager.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include "iniparse.h"
#include "DebugLogger.h"
#include "config.h"
#include <iomanip> // für std::setw, std::fixed, std::setprecision
#include <sstream>
#include <algorithm>
#include <vector>

H2OFilterManager::H2OFilterManager(Configuration& configRef) : config(configRef) {}




// Zentriert Text in Breite (für Überschriften)
std::string centerText(const std::string& text, int width) {
    int len = (int)text.length();
    if (len >= width) return text;
    int leftPadding = (width - len) / 2;
    int rightPadding = width - len - leftPadding;
    return std::string(leftPadding, ' ') + text + std::string(rightPadding, ' ');
}

// Druckt eine Linie mit Rand und - Zeichen
void printLine(int width) {
    std::cout << "  +" << std::string(width - 2, '-') << "+" << std::endl;
}

// Druckt eine Sektionüberschrift linksbündig mit Abstand zum Rand
void printSectionTitle(const std::string& title, int width) {
    std::cout << "  | " << std::left << std::setw(width - 4) << title << " |" << std::endl;
}

// Druckt eine Datenzeile mit links Label und rechts Wert (rechtsbündig)
// labelWidth + valueWidth + 5 (für  |, spaces, | )
void printDataRow(const std::string& label, const std::string& value, int labelWidth, int valueWidth) {
    std::cout << "  | " << std::left << std::setw(labelWidth) << label  << std::right << std::setw(valueWidth) << value << " |" << std::endl;
}

// Format float mit n Nachkommastellen als String
std::string floatToStr(float val, int decimals = 2) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << val;
    return oss.str();
}

// Haupt-Ausgabefunktion
void H2OFilterManager::printStatistik() {
    // Boxbreite insgesamt
    const int boxWidth = 57;
    // Spaltenbreiten
    const int labelWidth = 40;
    const int valueWidth = boxWidth - labelWidth - 5; // 5 = "  | " + " |"
    printLine(boxWidth);
    std::cout << "  |" << centerText("STATISTIK", boxWidth - 2) << "|" << std::endl;
    printLine(boxWidth);
    // Filter Sektion
    printSectionTitle("Filter", boxWidth);
    printDataRow(" Leistung:", std::to_string((int)config.filter.filter_gpd) + " GPD", labelWidth+1, valueWidth);
    printDataRow(" Faktor (Filtermenge->Abwassermenge):          1 ->", std::to_string((int)config.filter.filter_verhaeltnis), labelWidth, valueWidth-10);
    printDataRow(" Bisherige Filtermenge:", floatToStr(config.filter.filter_total_liter) + " L", labelWidth+1, valueWidth);
    printDataRow(" Filterlimit für Filtertausch:", std::to_string((int)config.filter.filter_max_menge_filtern) + " L", labelWidth+2, valueWidth);
    printDataRow(" Restmenge bis Filtertausch:", floatToStr(config.filter.filter_max_menge_filtern - config.filter.filter_total_liter) + " L", labelWidth+1, valueWidth);
    printDataRow(" Spülzeit in Sekunden:", std::to_string(config.filter.filter_clean_time), labelWidth+2, valueWidth);
    printDataRow(" 12V ID von Ventil oder Pumpe:", std::to_string(config.filter.ventil_pumpe), labelWidth+1, valueWidth);
    printDataRow(" 12V ID von H³O² Generator:", std::to_string(config.filter.hex_generator), labelWidth+3, valueWidth);
    printLine(boxWidth);
    // Abwasser Sektion
    printSectionTitle("Abwasser", boxWidth);
    printDataRow(" Typ (Tank|Abfluss):", (config.tank.abwassertank_vorhanden == 1 ? "Tank" : "Abwasser"), labelWidth+1, valueWidth);
    printLine(boxWidth);
    // Tank Sektion
    printSectionTitle("Tank", boxWidth);
    printDataRow(" aktiv:", (config.tank.abwassertank_vorhanden == 1 ? "Ja" : "Nein"), labelWidth+1, valueWidth);
    if (config.tank.abwassertank_vorhanden == 1) {
        printDataRow(" Fassungsvermögen:", floatToStr(config.tank.abwassertank_groesse, 0) + " L", labelWidth+2, valueWidth);
        printDataRow(" Aktueller Füllstand:", floatToStr(config.tank.abwassertank_total_liter) + " L", labelWidth+2, valueWidth);
    } else {
        printDataRow(" Fassungsvermögen:", "-", labelWidth+2, valueWidth);
        printDataRow(" Aktueller Füllstand:", "-", labelWidth+2, valueWidth);
    }
    printLine(boxWidth);
    std::cout << "\n" << std::endl;
}


void H2OFilterManager::startReinigung() {
    std::cout << "-> Spülvorgang beginnt...\n";
    sprintf(command, "12V %d 1 0", config.filter.ventil_pumpe);
    system(command);
    sprintf(command, "12V %d 0 %d &", config.filter.ventil_pumpe, config.filter.filter_clean_time);
    system(command);
    for (int i = 0; i <= config.filter.filter_clean_time; ++i) {
        int bar_width = 53;
        float progress = static_cast<float>(i) / config.filter.filter_clean_time;
        int bar_length = progress * bar_width;
        std::cout << "\r-> [" << std::string(bar_length, '#') << std::string(bar_width - bar_length, ' ') << "] " << std::fixed << std::setprecision(2) << progress * 100 << "%";
        std::cout.flush();
        sleep(1);
    }
    std::cout << "\n-> Spülung abgeschlossen.\n";
}



void H2OFilterManager::startFilterwechsel() {
    std::cout << "-> Filterwechsel\n";
    sprintf(command, "bash /Energiebox/h2o/setIni2.sh %d %d %d %d %d %d %d %f %d %d %f", 
            config.filter.ventil_pumpe, 
            config.filter.hex_generator, 
            config.filter.filter_gpd, 
            config.filter.filter_max_menge_filtern, 
            config.filter.filter_verhaeltnis, 
            config.filter.filter_clean_time, 
            config.filter.filter_zeit_100ml, 
            0.00f,  /* config.filter.filter_total_liter */   /* float */
            config.tank.abwassertank_vorhanden, 
            config.tank.abwassertank_groesse, 
            config.tank.abwassertank_total_liter  /* float */
    );
    system(command);
    std::cout << "-> Filter wurde gewechselt.\n";
}

void H2OFilterManager::leereAbwassertank() {
    std::cout << "-> Abwasser-Zähler wird zurückgesetzt...\n";
    sprintf(command, "bash /Energiebox/h2o/setIni2.sh %d %d %d %d %d %d %d %f %d %d %f", 
            config.filter.ventil_pumpe, 
            config.filter.hex_generator, 
            config.filter.filter_gpd, 
            config.filter.filter_max_menge_filtern, 
            config.filter.filter_verhaeltnis, 
            config.filter.filter_clean_time, 
            config.filter.filter_zeit_100ml, 
            config.filter.filter_total_liter, /* float */
            config.tank.abwassertank_vorhanden, 
            config.tank.abwassertank_groesse, 
            0.00f /* config.tank.abwassertank_total_liter */  /* float */
    );
    system(command);
    std::cout << "-> Tank ist jetzt leer.\n";
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









