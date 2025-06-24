#include "BatteryStatus.hpp"
#include "PVStatus.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

void
printUsage()
{
    std::cout << "Nutzung: epever [Option]\n"
              << "Optionen:\n"
              << "  -l, --loop      Wiederholt die Anzeige alle 2 Sekunden\n"
              << "  -h, --help      Zeigt diese Hilfe an\n";
}

int
main(int argc, char *argv[])
{
    bool loop = false;

    // Parameter checken .
    if(argc > 1) {
        std::string arg = argv[1];
        if(arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        }
        else if(arg == "--loop" || arg == "-l") {
            loop = true;
        }
        else {
            std::cerr << "Unbekannte Option: " << arg << "\n\n";
            printUsage();
            return 1;
        }
    }

    PVStatus pv;
    BatteryStatus battery;

    do {
        if(!StatusBlock::loadTraceFile("/Energiebox/Tracer/trace.txt")) {
            std::cerr << "Fehler: trace.txt konnte nicht geladen werden\n";
            return 1;
        }
        if(!pv.update() || !battery.update()) {
            std::cerr << "Fehler beim Lesen der Daten\n";
            return 1;
        }
        StatusBlock::clearScreen();
        pv.draw();
        battery.draw();
        if(loop) {
            std::this_thread::sleep_for(std::chrono::seconds(180));
        }
    } while(loop);
    return 0;
}
