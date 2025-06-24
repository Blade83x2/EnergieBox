#include "PVStatus.hpp"
#include "StatusBlock.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

/**
 * @brief Liest die PV-bezogenen Werte aus der geladenen traceData und aktualisiert die
 * Member-Variablen.
 *
 * Es werden folgende Daten extrahiert:
 * - Spannung (V)
 * - Strom (A)
 * - Leistung (W)
 * - Heute generierte Energie (kWh)
 *
 * @return true Immer true, da aktuell keine Fehlerbehandlung für fehlende Daten erfolgt.
 */

bool PVStatus::update() {
    voltage = 0.0f;
    current = 0.0f;
    power = 0.0f;
    generatedToday = 0.0f;

    // Prüfen, ob die traceData leer ist (d.h. Datei wurde nicht gelesen)
    if (traceData.empty()) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        StatusBlock::loadTraceFile("/Energiebox/Tracer/trace.txt");
    }

    // Nach Wartezeit erneut prüfen
    if (traceData.empty()) {
        std::cerr << "[FEHLER] trace.txt war mindestens 10 Sekunden lang leer.\n";
        return false;
    }

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
        std::cout.width(8); // feste Spaltenbreite
        std::cout << std::left << oss.str();
    }
    printBar(voltage, 150.0f); // Max: 150V
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
    printBar(current, 100.0f); // Max: 100A
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
    printBar(power, 5000.0f); // Max: 5kW
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
