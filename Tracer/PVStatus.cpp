#include "PVStatus.hpp"

bool PVStatus::update() {
    FILE* fp = popen("python3 /Energiebox/Tracer/readall.py 2>&1", "r");
    if (!fp) return false;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "PV Array: Aktuelle Spannung in Volt =")) voltage = extractValue(line);
        else if (strstr(line, "PV Array: Aktueller Strom in Ampere =")) current = extractValue(line);
        else if (strstr(line, "PV Array: Aktuelle Leistung in Watt =")) power = extractValue(line);
        else if (strstr(line, "PV Array: Generierte Energie heute =")) generatedToday = extractValue(line);
    }
    pclose(fp);
    return true;
}

void PVStatus::draw() const {
    printHeader("PV Status");

    std::cout.width(WIDTH_LABEL); std::cout << std::left << "Spannung:" << " ";
    printFloat(voltage, "V");
    std::cout << " ";
    printBar(voltage, 150.0f);
    std::cout << "\n";

    std::cout.width(WIDTH_LABEL); std::cout << std::left << "Strom:" << " ";
    printFloat(current, "A");
    std::cout << " ";
    printBar(current, 100.0f);
    std::cout << "\n";

    std::cout.width(WIDTH_LABEL); std::cout << std::left << "Leistung:" << " ";
    printFloat(power, "W");
    std::cout << " ";
    printBar(power, 5000.0f);
    std::cout << "\n";

    std::cout.width(WIDTH_LABEL); std::cout << std::left << "Heute Generiert:" << " ";
    printFloat(generatedToday, "kWh", false, false);
    std::cout << "\n\n\n";
}
