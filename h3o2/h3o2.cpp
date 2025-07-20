/*
 * Konsolen Programm zur Steuerung der Wasserfilterung
 * Vendor: Johannes Krämer
 * Version: 1.0
 * Date: 20.07.2025
 *
 */
#include <iostream>
#include "config.h"
#include "DebugLogger.h"
#include "H2OSetupManager.h"
#include "H2OFilterManager.h"
#include "JOHNNYSWATER.h"
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <cmath>
#include <limits>

Config* config = nullptr;

/**
 * Signal-Handler-Funktion: Aufruf bei STRG+C
 **/
void handleSigint(int signal) {
    if (signal == SIGINT) {
        // Relais deaktivieren
        int ventil_pumpe = config->get<int>("filter/ventil_pumpe");
        std::string command = "12V " + std::to_string(ventil_pumpe) + " 0 0";
        system(command.c_str());
        std::cout << "\n  \e[0;31m-> Programm wurde mit Strg + C unterbrochen!\n\n\e[0m";
        std::exit(EXIT_SUCCESS);
    }
}

/**
 * Programmstart
 **/
int main(int argc, char* argv[]) {
    // Signal-Handler registrieren
    std::signal(SIGINT, handleSigint);
    JOHNNYSWATER app(argc, argv);
    app.init();
    config = app.getConfig();
    app.run();
    return EXIT_SUCCESS;
}