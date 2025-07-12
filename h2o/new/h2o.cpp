#include <iostream>
#include "config.h"
#include "DebugLogger.h"
#include "H2OSetupManager.h"
#include "H2OFilterManager.h"
#include "JOHNNYSWATER.h"
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <cmath>
#include <limits>

int main(int argc, char* argv[]) {
    JOHNNYSWATER app(argc, argv);
    app.init();
    app.run();
    return 0;
}
