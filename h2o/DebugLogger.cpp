#include "DebugLogger.h"
#include <iostream>
#include <iomanip>
#include <ctime>

bool DebugLogger::debugEnabled = false;

void DebugLogger::setEnabled(bool enabled) {
    debugEnabled = enabled;
}

void DebugLogger::log(const std::string& message, LogLevel level) {
    if (!debugEnabled) return;

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::cout << "["
              << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
              << "] [" << levelToString(level) << "] "
              << message << std::endl;
}

std::string DebugLogger::levelToString(LogLevel level) {
    switch (level) {
        case INFO: return "INFO";
        case WARN: return "WARN";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
