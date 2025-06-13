#include "BatteryStatus.hpp"

bool BatteryStatus::update() {
    FILE* fp = popen("python3 /Energiebox/Tracer/readall.py 2>&1", "r");
    if (!fp) return false;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Batterie: Ladezustand in Prozent =")) soc = extractValue(line);
        else if (strstr(line, "Batterie: Aktuelle Spannung in Volt =")) voltage = extractValue(line);
    }
    pclose(fp);
    return true;
}

void BatteryStatus::draw() const {
    printHeader("Batterie Status");

    std::cout.width(WIDTH_LABEL); std::cout << std::left << "Ladezustand:" << " ";
    printFloat(soc, "%", true);
    std::cout << " ";
    printBar(soc, 100.0f);
    std::cout << "\n";

    std::cout.width(WIDTH_LABEL); std::cout << std::left << "Spannung:" << " ";
    printFloat(voltage, "V");
    std::cout << " ";
    printBar(voltage - 48.0f, 8.0f);
    std::cout << "\n\n";
}
