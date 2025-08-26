#include "BatteryStatus.hpp"
#include "PVStatus.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

// Build-Infos (werden vom Makefile via -D gesetzt)
#ifndef BUILD_DATE
#define BUILD_DATE "unbekannt"
#endif

#ifndef BUILD_VERSION
#define BUILD_VERSION "1.0"
#endif

void printUsage() {
    std::cout << "Nutzung: status [Option]\n"
              << "Optionen:\n"
              << "  -l, --loop      Wiederholt die Anzeige jede Sekunde\n"
              << "  -v, --version   Zeigt die Versionsinformationen an\n"
              << "  -h, --help      Zeigt diese Hilfe an\n\n";
}

void printVersion() {
    std::cout << "status Version " << BUILD_VERSION << " (Build am " << BUILD_DATE << ")\n\n";
}

int main(int argc, char *argv[]) {
    bool loop = false;

    // Parameter checken .
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 1;
        } else if (arg == "--loop" || arg == "-l") {
            loop = true;
        } else {
            std::cerr << "Unbekannte Option: " << arg << "\n\n";
            printUsage();
            return 1;
        }
    }

    PVStatus pv;
    BatteryStatus battery;

    do {
        StatusBlock::clearScreen();
        pv.update().draw();
        battery.update().draw();
        if (loop) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } while (loop);
    return 0;
}
