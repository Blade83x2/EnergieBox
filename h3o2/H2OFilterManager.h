#ifndef H2OFILTERMANAGER_H
#define H2OFILTERMANAGER_H
#include "H2OSetupManager.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include "DebugLogger.h"
#include <iomanip>  // für std::setw, std::fixed, std::setprecision
#include <sstream>
#include <algorithm>
#include <vector>

class Config;  // Forward Declaration

class H2OFilterManager {
   public:
    explicit H2OFilterManager(Config& configRef);

    void sendToBash(const std::string& command);
    void filter(double menge, bool silentMode);
    int berechneSekundenAusFiltermege(double wassermenge);
    double berechneAbwasserMenge(double wassermenge);
    bool berechneFreieAbwasserMengeImTank(double abwassermenge);
    double berechneLiterMengeAusZeit(double laufzeit);
    void addiereAbwasser(double abwasserMengeInLiter);
    void addiereGefiltertesWasser(double filterMengeInLiter);
    void startReinigung();
    void startFilterwechsel();
    void leereAbwassertank();
    void printStatistik();
    static std::string floatToStr(float val, int decimals = 2);
    static std::string centerText(const std::string& text, int width);
    static void printLine(int width);
    static void printSectionTitle(const std::string& title, int width);
    static void printDataRow(const std::string& label, const std::string& value, int labelWidth, int valueWidth);

    // Fortschrittsbalken mit variabler Breite
    void printProgress(const std::string& prefix, size_t count, size_t max, const int bar_width);

    /*  Koma in Punkt umtauschen    H2OFilterManager::replace_char(std::string menge = "12,34", ',', '.');  */
    static void replace_char(std::string& s, char find, char replace) {
        for (char& c : s) {
            if (c == find) c = replace;
        }
    }

    std::string formatSecondsToHMS(int seconds);

    bool silentMode = false;
    void setSilentMode(bool silent) { silentMode = silent; }

   private:
    Config& config;

    void printIfNotSilent(const std::string& message) {
        if (!silentMode) {
            std::cout << message << std::flush;
        }
    }

    bool isSilent() {
        if (silentMode) {
            return true;
        }
        return false;
    }
};

#endif  // H2OFILTERMANAGER_H
