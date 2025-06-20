#ifndef H2OFILTERMANAGER_H
#define H2OFILTERMANAGER_H

#include "iniparse.h"
#include "DebugLogger.h"
#include "config.h"
#include <string>

class H2OFilterManager {
public:
    H2OFilterManager(Configuration& configRef);

    void startReinigung();
    void startFilterwechsel();
    void leereAbwassertank();

private:
    Configuration& config;
    char command[256];
};

#endif // H2OFILTERMANAGER_H
