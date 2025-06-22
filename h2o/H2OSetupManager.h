#ifndef H2OSETUPMANAGER_H
#define H2OSETUPMANAGER_H

#include "H2OFilterManager.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include "iniparse.h"
#include "DebugLogger.h"
#include "config.h"
#include <iomanip> // für std::setw, std::fixed, std::setprecision
#include <sstream>
#include <algorithm>
#include <vector>


class H2OSetupManager {

public:
    H2OSetupManager(Configuration& configRef);
    
    void runSetup();
    
    
private:
    Configuration& config;
    char command[256];

    
};
#endif // H2OSETUPMANAGER_H
