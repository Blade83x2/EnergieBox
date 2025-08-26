#include "PVStatus.hpp"
#include "StatusBlock.hpp"
#include "MySQLiWrapper.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

/**
 * @brief Liest die PV-bezogenen Werte aus der Datenbank
 *
 * Es werden folgende Daten extrahiert:
 * - Spannung (V)
 * - Strom (A)
 * - Leistung (W)
 * - Heute generierte Energie (kWh)
 *
 * @return true Immer true, da aktuell keine Fehlerbehandlung für fehlende Daten erfolgt.
 */
PVStatus& PVStatus::update() {
    MySQLiWrapper db("/home/box/.mysql_energiebox.cfg");
    voltage = 0.0f;
    current = 0.0f;
    power = 0.0f;
    generatedToday = 0.0f;
    if (db.query("SELECT pv_volt, pv_ampere, pv_power, generated_power FROM messwerte ORDER BY id DESC LIMIT 1")) {
        std::map<std::string, std::string> row = db.fetchArray();
        // PV Daten
        voltage = std::stof(row["pv_volt"]);
        current = std::stof(row["pv_ampere"]);
        power = std::stof(row["pv_power"]);
        generatedToday = std::stof(row["generated_power"]);
    }
    return *this;
}

/**
 * @brief Gibt die aktuellen PV-Daten formatiert im Terminal aus, inklusive ASCII-Balken.
 *
 * Es werden alle Werte (Spannung, Strom, Leistung, Energie heute) mit:
 * - Einheit
 * - Fester Breite
 * - ASCII-Fortschrittsbalken (bei V, A, W)
 */
void PVStatus::draw() const {
    printHeader("PV Status");

    // Spannung
    std::cout.width(WIDTH_LABEL);
    std::cout << std::left << "Spannung:"
              << " ";

    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << voltage << "V";
        std::cout.width(8);  // feste Spaltenbreite
        std::cout << std::left << oss.str();
    }
    printBar(voltage, 150.0f);  // Max: 150V
    std::cout << "\n";

    // Strom
    std::cout.width(WIDTH_LABEL);
    std::cout << std::left << "Strom:"
              << " ";

    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << current << "A";
        std::cout.width(8);
        std::cout << std::left << oss.str();
    }
    printBar(current, 100.0f);  // Max: 100A
    std::cout << "\n";

    // Leistung
    std::cout.width(WIDTH_LABEL);
    std::cout << std::left << "Leistung:"
              << " ";

    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << power << "W";
        std::cout.width(8);
        std::cout << std::left << oss.str();
    }
    printBar(power, 5000.0f);  // Max: 5kW
    std::cout << "\n";

    // Energie heute
    std::cout.width(WIDTH_LABEL);
    std::cout << std::left << "Heute Generiert:"
              << " ";

    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << generatedToday << "kWh";
        std::cout.width(8);
        std::cout << std::left << oss.str();
    }

    std::cout << "\n\n\n";
}
