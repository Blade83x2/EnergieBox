#ifndef H2OSETUPMANAGER_H
#define H2OSETUPMANAGER_H

class H2OSetupManager {
private:
    char command[256];

public:
    // Führt den interaktiven Setup-Prozess durch
    void runSetup();
};

#endif // H2OSETUPMANAGER_H
