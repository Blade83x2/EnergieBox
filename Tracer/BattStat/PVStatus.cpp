#include "PVStatus.hpp"
#include <sstream>
#include <iostream>

bool PVStatus::update() {
    voltage = 0.0f;
    current = 0.0f;
    power = 0.0f;
    generatedToday = 0.0f;

    std::istringstream stream(traceData);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("PV Array: Aktuelle Spannung in Volt =") != std::string::npos)
            voltage = extractValue(line);
        else if (line.find("PV Array: Aktueller Strom in Ampere =") != std::string::npos)
            current = extractValue(line);
        else if (line.find("PV Array: Aktuelle Leistung in Watt =") != std::string::npos)
            power = extractValue(line);
        else if (line.find("PV Array: Generierte Energie heute =") != std::string::npos)
            generatedToday = extractValue(line);
    }
    return true;
}

void PVStatus::draw() const {
    printHeader("PV Status");

    std::cout.width(WIDTH_LABEL);
    std::cout << std::left << "Spannung:"
              << " ";
    printFloat(voltage, "V");
    std::cout << " ";
    printBar(voltage, 150.0f);
    std::cout << "\n";

    std::cout.width(WIDTH_LABEL);
    std::cout << std::left << "Strom:"
              << " ";
    printFloat(current, "A");
    std::cout << " ";
    printBar(current, 100.0f);
    std::cout << "\n";

    std::cout.width(WIDTH_LABEL);
    std::cout << std::left << "Leistung:"
              << " ";
    printFloat(power, "W");
    // std::cout << " ";
    printBar(power, 5000.0f);
    std::cout << "\n";

    std::cout.width(WIDTH_LABEL);
    std::cout << std::left << "Heute Generiert:"
              << " ";
    printFloat(generatedToday, "kWh", false, false);
    std::cout << "\n\n\n";
}
