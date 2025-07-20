#include "DebugLogger.h"
#include <fstream>

#include <iostream>
#include <iomanip>
#include <ctime>

bool DebugLogger::debugEnabled = false;

void DebugLogger::log(const std::string& message) {
    if (!debugEnabled) return;
    std::ofstream file("/Energiebox/error.log", std::ios::app);
    if (file.is_open()) {
        file << message << std::endl;
    }
}
void DebugLogger::setDebugMode(bool enable) {
    debugEnabled = enable;
}
