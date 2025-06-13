#include "StatusBlock.hpp"

float StatusBlock::extractValue(const char* line) {
    float val = 0.0f;
    sscanf(line, "%*[^=]= %f", &val);
    return val;
}

void StatusBlock::printBar(float value, float maxValue) const {
    int width = WIDTH_BAR - 2;
    int percent = static_cast<int>((value / maxValue) * 100);
    if (percent > 100) percent = 100;
    if (percent < 0) percent = 0;
    int filled = (percent * width) / 100;

    std::cout << "[";
    for (int i = 0; i < width; ++i) std::cout << (i < filled ? "█" : "░");
    std::cout << "]";
}

void StatusBlock::printFloat(float value, const char* unit, bool color, bool rightJustified) const {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f%s", value, unit);
    int len = strlen(buffer);
    int padding = WIDTH_VALUE - len;
    if (rightJustified) for (int i = 0; i < padding; ++i) std::cout << " ";

    if (color) {
        const char* colorCode = value < 20.0f ? "\033[1;31m" : "\033[1;32m";
        std::cout << colorCode;
    } else {
        std::cout << "\033[1;37m";
    }

    std::cout << buffer << "\033[0m";
}

void StatusBlock::clearScreen() {
    std::cout << "\033[2J\033[H";
}

void StatusBlock::printHeader(const char* title) {
    std::cout << "\033[1;44;97m";
    int len = strlen(title);
    int leftPad = (TOTAL_WIDTH - len) / 2;
    int rightPad = TOTAL_WIDTH - leftPad - len;
    for (int i = 0; i < leftPad; ++i) std::cout << " ";
    std::cout << title;
    for (int i = 0; i < rightPad; ++i) std::cout << " ";
    std::cout << "\033[0m\n\n";
}
