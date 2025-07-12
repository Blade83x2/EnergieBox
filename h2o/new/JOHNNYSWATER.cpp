#include "JOHNNYSWATER.h"
#include <iostream>
#include <algorithm>

JOHNNYSWATER::JOHNNYSWATER(int argc_, char* argv_[]) : argc(argc_), argv(argv_) {
    // Konstruktor-Logik (optional)
}
JOHNNYSWATER::~JOHNNYSWATER() {
    delete config;
    delete setupManager;
    delete filterManager;
    delete debugLogger;
}

void JOHNNYSWATER::init() {
    this->config = new Config("/Energiebox/h2o/tesst/config.ini");
    this->debugLogger = new DebugLogger();
    this->setupManager = new H2OSetupManager(*config);
    this->filterManager = new H2OFilterManager(*config);
}

void JOHNNYSWATER::printHelp() const {
    std::cout << "\n=== JOHNNYSWATER Steuerung ===\n\n"
              << "Verwendung:\n"
              << "  h2o_test [Option]\n\n"
              << "Optionen:\n"
              << "  -setup, -s               Einrichtung starten\n"
              << "  -filter, -f, --filter    Filtervorgang starten\n"
              << "  -clean, -c, --clean      Spülvorgang starten\n"
              << "  -empty, -e, --empty      Abwassertank leeren\n"
              << "  -stats, -t, --stats      Statistik anzeigen\n"
              << "  -speedtest, -s           Speedtest starten\n"
              << "  -silent, --silent        Ausgabe unterdrücken\n"
              << "  -help, -h, --help        Diese Hilfe anzeigen\n"
              << std::endl;
}

void JOHNNYSWATER::getOps() {
    if (argc < 2) {
        printHelp();
        this->command.clear();
        return;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);

        if (arg == "-silent" || arg == "-" || arg == "--silent") {
            silentMode = true;
        } else if (arg == "-setup" || arg == "-s" || arg == "--silent") {
            this->command = "setup";
        } else if (arg == "-filter" || arg == "-f" || arg == "--filter") {
            this->command = "filter";
        } else if (arg == "-clean" || arg == "-c" || arg == "--clean") {
            this->command = "clean";
        } else if (arg == "-empty" || arg == "-e" || arg == "--empty") {
            this->command = "empty";
        } else if (arg == "-stats" || arg == "-t" || arg == "--stats") {
            this->command = "stats";
        } else if (arg == "-speedtest" || arg == "-s" || arg == "--speedtest") {
            this->command = "speedtest";
        } else if (arg == "-help" || arg == "-h" || arg == "?") {
            printHelp();
            this->command.clear();
            return;
        } else {
            std::cerr << "Unbekannter Befehl: " << arg << "\n";
            this->command.clear();
            return;
        }
    }

    filterManager->silentMode = silentMode;
    debugLogger->setSilentMode(silentMode);
}

void JOHNNYSWATER::run() {
    this->setupManager->sendToBash(std::string("clear"));
    this->setupManager->sendToBash(std::string("figlet -f mono12 ' H3O2'"));
    this->getOps();

    if (this->command.empty()) return;

    if (this->command == "setup") {
        this->setupManager->runSetup();
    } else if (this->command == "filter") {
        this->filterManager->startFilterwechsel();
    } else if (this->command == "clean") {
        this->filterManager->startReinigung();
    } else if (this->command == "empty") {
        this->filterManager->leereAbwassertank();
    } else if (this->command == "stats") {
        this->filterManager->printStatistik();
    } else if (this->command == "speedtest")
        this->setupManager->runSpeedtest();
}
