#include "JOHNNYSWATER.h"
#include <iostream>
#include <algorithm>

/**
 * Konstruktor
 */
JOHNNYSWATER::JOHNNYSWATER(int argc_, char* argv_[]) : argc(argc_), argv(argv_) {
    // Konstruktor-Logik (optional)
}

/**
 * Destruktor
 */
JOHNNYSWATER::~JOHNNYSWATER() {
    delete config;
    delete setupManager;
    delete filterManager;
    delete debugLogger;
}

/**
 * Programm Initialisierung
 */
void JOHNNYSWATER::init() {
    this->config = new Config("/Energiebox/h3o2/config.ini");
    this->debugLogger = new DebugLogger();
    this->setupManager = new H2OSetupManager(*config);
    this->filterManager = new H2OFilterManager(*config);
}

/**
 * Gibt config zurück.
 *
 */
Config* JOHNNYSWATER::getConfig() {
    return this->config;
}

/**
 * Zeigt eine Hilfestellung an wie das Programm zu benutzen ist
 */
void JOHNNYSWATER::printHelp() const {
    this->setupManager->sendToBash(std::string("clear"));
    this->setupManager->sendToBash(std::string("figlet -f mono12 ' H3O2'"));
    std::cout << "\n Dieses Programm filtert Wasser. Danach wird es mit bestimmten\n Frequenzen beschallt damit das Wasser eine Hexagonale Struktur aufweisst.\n\n"
              << " Verwendung:\n"
              << "  h3o2 3,3                    Filtert 3,3 Liter Wasser\n"
              << "  h3o2 .7 -silent             Filtert 0,7 Liter Wasser ohne Anzeige\n\n"
              << " Optionen:\n"
              << "  -setup,     --setup         Einrichtung starten\n"
              << "  -change,    --change        Filter tauschen\n"
              << "  -clean,     --clean         Spülvorgang starten\n"
              << "  -empty,     --empty         Abwassertank leeren\n"
              << "  -stats,     --stats         Statistik anzeigen\n"
              << "  -speedtest, --speedtest     Filterzeit einstellen\n"
              << "  -silent,    --silent        Ausgabe unterdrücken\n"
              << "  -help,      --help          Diese Hilfe anzeigen\n"
              << std::endl;
}

/**
 * Ermittelt übergebene Programm Parameter
 */
void JOHNNYSWATER::getOps() {
    if (argc < 2) {
        printHelp();
        this->command.clear();
        return;
    }
    double menge = 0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
        if (arg == "-silent" || arg == "--silent") {
            this->silentMode = true;
        } else if (arg == "-setup" || arg == "--setup") {
            this->command = "setup";
        } else if (arg == "-change" || arg == "--change") {
            this->command = "change";
        } else if (arg == "-clean" || arg == "--clean") {
            this->command = "clean";
        } else if (arg == "-empty" || arg == "--empty") {
            this->command = "empty";
        } else if (arg == "-stats" || arg == "--stats") {
            this->command = "stats";
        } else if (arg == "-speedtest" || arg == "--speedtest") {
            this->command = "speedtest";
        } else if (arg == "-help" || arg == "--help") {
            printHelp();
            this->command.clear();
            return;
        } else {
            try {                                               // Prüfen ob numerischer Wert (int, float, double)
                H2OFilterManager::replace_char(arg, ',', '.');  // Falls ein "," vorhanden ist, mit "." ersetzen
                menge = std::stod(arg);                         // Versuch, den Wert umzuwandeln
            } catch (const std::invalid_argument& e) {
                std::cerr << "  \e[0;31m-> Unbekannter Parameter: " << arg << "\n\n\e[0m";
                this->command.clear();
                return;
            }
        }
    }
    if (menge > 0) {
        filterManager->filter(menge, this->silentMode);
        return;
    }
    // SilentMode an andere Komponenten weitergeben
    filterManager->silentMode = this->silentMode;
    debugLogger->setSilentMode(this->silentMode);
}

/**
 * Programmstart
 */
void JOHNNYSWATER::run() {
    this->getOps();  // Parameter ermitteln
    // Aktion aufrufen, je nach Parameter
    if (this->command == "setup") {
        this->setupManager->runSetup();
    } else if (this->command == "change") {
        this->filterManager->startFilterwechsel();
    } else if (this->command == "clean") {
        this->filterManager->startReinigung();
    } else if (this->command == "empty") {
        this->filterManager->leereAbwassertank();
    } else if (this->command == "stats") {
        this->filterManager->printStatistik();
    } else if (this->command == "speedtest") {
        this->setupManager->runSpeedtest();
    }
}
