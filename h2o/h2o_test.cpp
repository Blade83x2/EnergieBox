// H2O Osmose Filter Controller
// Vendor: Johannes Krämer
// Version: 1.6 (C++ Rewrite)
// Date: 19.06.2025

//  sudo g++ -o h2o_test h2o_test.cpp H2OSetupManager.cpp DebugLogger.cpp H2OFilterManager.cpp config.cpp -std=c++17

#include <iostream>
#include "config.h"
#include "DebugLogger.h"
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

int main(int argc, char* argv[]) {
    DebugLogger::setEnabled(true);

    Config config("/Energiebox/h2o/config.ini");

    int ventil_pumpe = config.get<int>("filter/ventil_pumpe");

    config.set<int>("filter/ventil_pumpe", 5);

    // Sicherstellen, dass Pumpe aus ist
    printf("lkjlkj %d ", ventil_pumpe);

    int paramCount = argc;
    printf(" %d ", paramCount);
    std::string param = argv[1];
    if (param == "-clean") {
        H2OFilterManager filter(config);
        filter.startReinigung();

    } else if (param == "-change") {
        H2OFilterManager filter(config);
        filter.startFilterwechsel();
    } else if (param == "-empty") {
        H2OFilterManager filter(config);
        filter.leereAbwassertank();
    } else if (param == "-setup") {
        H2OSetupManager setup(config);
        setup.runSetup();
    } else if (param == "-stats") {
        H2OFilterManager filter(config);
        filter.printStatistik();
    } else {
    }

    return 0;
}
