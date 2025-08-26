#include "BatteryStatus.hpp"
#include "StatusBlock.hpp"
#include "MySQLiWrapper.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

BatteryStatus& BatteryStatus::update() {
    MySQLiWrapper db("/home/box/.mysql_energiebox.cfg");
    soc = 0.0f;
    voltage = 0.0f;
    if (db.query("SELECT batt_volt, batt_soc FROM messwerte ORDER BY id DESC LIMIT 1")) {
        std::map<std::string, std::string> row = db.fetchArray();
        // Batterie Daten
        soc = std::stof(row["batt_soc"]);
        voltage = std::stof(row["batt_volt"]);
    }
    return *this;
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
