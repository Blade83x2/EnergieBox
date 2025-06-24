#ifndef DEBUG_LOGGER_H
#define DEBUG_LOGGER_H

#include <string>

class DebugLogger {
   public:
    enum LogLevel { INFO, WARN, ERROR };

    static void setEnabled(bool enabled);
    static void log(const std::string& message, LogLevel level = INFO);

   private:
    static bool debugEnabled;
    static std::string levelToString(LogLevel level);
};

#endif  // DEBUG_LOGGER_H
