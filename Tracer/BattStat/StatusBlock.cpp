#include "StatusBlock.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

std::string StatusBlock::traceData;

float StatusBlock::extractValue(const std::string& line) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) return 0.0f;
    std::string valStr = line.substr(pos + 1);
    try {
        return std::stof(valStr);
    } catch (...) {
        return 0.0f;
    }
}

bool StatusBlock::loadTraceFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    traceData = buffer.str();

    return true;
}

void StatusBlock::clearScreen() {
    // ANSI Escape Code zum Bildschirm löschen
    std::cout << "\033[2J\033[H";
}

void StatusBlock::printHeader(const std::string& header) {
    std::cout << "=== " << header << " ===\n\n";
}

void StatusBlock::printFloat(float value, const char* unit, bool showPercent, bool showSpace) {
    if (showPercent)
        std::cout << std::fixed << std::setprecision(1) << value << "% ";
    else
        std::cout << std::fixed << std::setprecision(2) << value << unit;
    if (showSpace) std::cout << " ";
}

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
