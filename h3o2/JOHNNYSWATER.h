#ifndef JOHNNYSWATER_H
#define JOHNNYSWATER_H

#include <string>
#include "config.h"
#include "H2OSetupManager.h"
#include "H2OFilterManager.h"
#include "DebugLogger.h"

class JOHNNYSWATER {
   public:
    JOHNNYSWATER(int argc, char* argv[]);
    ~JOHNNYSWATER();

    void init();
    void run();
    Config* getConfig();

   private:
    void getOps();
    void printHelp() const;

    int argc;
    char** argv;

    bool silentMode = false;
    std::string command;

    Config* config = nullptr;
    H2OSetupManager* setupManager = nullptr;
    H2OFilterManager* filterManager = nullptr;
    DebugLogger* debugLogger = nullptr;
};

#endif  // JOHNNYSWATER_H
