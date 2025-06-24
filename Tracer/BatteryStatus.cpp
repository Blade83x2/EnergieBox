#include "BatteryStatus.hpp"
#include "StatusBlock.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

bool BatteryStatus::update() {
    soc = 0.0f;
    voltage = 0.0f;

    std::istringstream stream(traceData);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("Batterie: Ladezustand in Prozent =") != std::string::npos)
            soc = extractValue(line);
        else if (line.find("Batterie: Aktuelle Spannung in Volt =") != std::string::npos)
            voltage = extractValue(line);
    }
    return true;
}

void BatteryStatus::draw() const {
    printHeader("Batterie Status");

    std::cout.width(WIDTH_LABEL);
    std::cout << std::left << "Ladezustand:"
              << " ";
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << soc << "%";
        std::cout.width(8);  // feste Breite
        std::cout << std::left << oss.str();
    }
    printBar(soc, 100.0f);
    std::cout << "\n";

    std::cout.width(WIDTH_LABEL);
    std::cout << std::left << "Spannung:"
              << " ";
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << voltage << "V";
        std::cout.width(8);
        std::cout << std::left << oss.str();
    }
    printBar(voltage - 48.0f, 8.0f);
    std::cout << "\n\n";
}
