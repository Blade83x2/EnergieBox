#ifndef H2OFILTERMANAGER_H
#define H2OFILTERMANAGER_H

#include "H2OSetupManager.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include "iniparse.h"
#include "DebugLogger.h"
#include "config.h"
#include <iomanip>  // für std::setw, std::fixed, std::setprecision
#include <sstream>
#include <algorithm>
#include <vector>

// Werden für die Statistikanzeige benötigt
std::string centerText(const std::string& text, int width);
void printLine(int width);
void printSectionTitle(const std::string& title, int width);
void printDataRow(const std::string& label, const std::string& value, int labelWidth, int valueWidth);
std::string floatToStr(float val, int decimals);

class H2OFilterManager {
   public:
    H2OFilterManager(Configuration& configRef);

    void startReinigung();
    void startFilterwechsel();
    void leereAbwassertank();
    void printStatistik();

    // Zum Berechnen von Filterzeiten
    double gpdToLitersPerSecond(double gpd);
    int filterDauerInSekunden(double literZiel, double gpd);
    std::string formatSecondsToHMS(int seconds);

   private:
    Configuration& config;
    char command[256];
};

#endif  // H2OFILTERMANAGER_H
