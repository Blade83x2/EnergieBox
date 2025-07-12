#ifndef DEBUG_LOGGER_H
#define DEBUG_LOGGER_H

#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

// DebugLogger debugLogger;
// debugLogger.setSilentMode(true);  // oder false, je nachdem

// debugLogger.logInfo("Programm startet");
// debugLogger.logError("Fehler beim Öffnen der Datei");

class DebugLogger {
   public:
    DebugLogger(const std::string& logfilePath = "/Energiebox/error.log") : silentMode(false), logfile(logfilePath, std::ios::app) {
        if (!logfile.is_open()) {
            std::cerr << "ERROR: Kann Logdatei nicht öffnen: " << logfilePath << std::endl;
        }
    }

    static bool debugEnabled;

    static void log(const std::string& message);
    static void setDebugMode(bool enable);

    void setSilentMode(bool silent) { silentMode = silent; }

    void logInfo(const std::string& msg) { log("INFO", msg); }

    void logError(const std::string& msg) { log("ERROR", msg); }

   private:
    bool silentMode;
    std::ofstream logfile;
    std::mutex mtx;

    void log(const std::string& level, const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx);
        std::string timestamp = getTimestamp();

        if (!silentMode) {
            std::cout << "[" << timestamp << "] [" << level << "] " << msg << std::endl;
        }

        if (logfile.is_open()) {
            logfile << "[" << timestamp << "] [" << level << "] " << msg << std::endl;
            logfile.flush();
        }
    }

    std::string getTimestamp() {
        using namespace std::chrono;
        auto now = system_clock::now();
        std::time_t now_c = system_clock::to_time_t(now);
        std::tm local_tm;
#ifdef _WIN32
        localtime_s(&local_tm, &now_c);
#else
        localtime_r(&now_c, &local_tm);
#endif
        std::ostringstream oss;
        oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

#endif  // DEBUG_LOGGER_H
