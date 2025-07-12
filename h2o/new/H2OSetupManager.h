#ifndef H2OSETUPMANAGER_H
#define H2OSETUPMANAGER_H
#include "H2OFilterManager.h"
#include "DebugLogger.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <iomanip>  // für std::setw, std::fixed, std::setprecision
#include <sstream>
#include <algorithm>
#include <vector>

class Config;  // Forward Declaration

class H2OSetupManager {
   public:
    explicit H2OSetupManager(Config& configRef);

    void sendToBash(const std::string& command);
    void runSetup();
    void runSpeedtest();

   private:
    Config& config;
};
#endif  // H2OSETUPMANAGER_H
