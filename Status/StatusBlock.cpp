#include "StatusBlock.hpp"
#include <fstream>
#include <iomanip>  // Für std::setprecision
#include <iostream>
#include <sstream>

/**
 * @brief Löscht den Terminalbildschirm mithilfe von ANSI-Escape-Codes.
 *        Cursor wird danach auf Position (0, 0) gesetzt.
 */
void StatusBlock::clearScreen() {
    std::cout << "\033[2J\033[H";
}

/**
 * @brief Gibt einen Titelblock mit Gleichheitszeichen zur optischen Abgrenzung aus.
 *
 * @param header Überschrift des folgenden Blocks (z. B. "PV Status").
 */
void StatusBlock::printHeader(const std::string &header) {
    std::cout << "=== " << header << " ===\n\n";
}

/**
 * @brief Gibt eine formatierte Gleitkommazahl mit Einheit aus.
 *
 * @param value Der Wert.
 * @param unit Einheit (z. B. "V", "kWh").
 * @param showPercent true: zeigt den Wert mit Prozentzeichen und einer Nachkommastelle.
 * @param showSpace true: hängt ein Leerzeichen nach der Einheit an.
 */
void StatusBlock::printFloat(float value, const char *unit, bool showPercent, bool showSpace) {
    if (showPercent)
        std::cout << std::fixed << std::setprecision(1) << value << "%";
    else
        std::cout << std::fixed << std::setprecision(2) << value << unit;

    if (showSpace) std::cout << " ";
}

/**
 * @brief Gibt einen Balken aus, der den relativen Wert zu einem Maximalwert visualisiert.
 *
 * @param value Aktueller Wert.
 * @param maxValue Maximaler möglicher Wert (repräsentiert einen vollen Balken).
 */
void StatusBlock::printBar(float value, float maxValue) {
    const int barWidth = 30;
    int filled = static_cast<int>((value / maxValue) * barWidth);
    if (filled < 0) filled = 0;
    if (filled > barWidth) filled = barWidth;

    std::cout << "[";
    for (int i = 0; i < filled; i++) std::cout << "#";
    for (int i = filled; i < barWidth; i++) std::cout << " ";
    std::cout << "]";
}
