#include "RuleEngine.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cctype>

extern void speak(const std::string&);
extern int exec(const std::string&);
extern void remoteCommand(const std::string&, const std::string&);
extern void localCommand(const std::string&);

RuleEngine::RuleEngine(const std::vector<Rule>& r, std::map<std::string, std::string> config) : rules(r), ssh_host(config["ssh_remote_addr"]) {}

// Durchläuft alle Gruppen aus yaml Datei und sucht übereinstimmung
bool RuleEngine::match(const Rule& rule, const std::string& input) {
    for (auto& group : rule.conditions) {
        bool ok = false;
        for (auto& w : group) {
            if (input.find(w) != std::string::npos) {
                ok = true;
                break;
            }
        }
        if (!ok) return false;
    }
    return true;
}

// Führt command aus
void RuleEngine::process(const std::string& input) {
    for (auto& r : rules) {
        if (match(r, input)) {
            execute(r, input);
            return;
        }
    }
}

// Liesst command & speak aus yaml Datei und führt es aus
void RuleEngine::execute(const Rule& rule, const std::string&) {
    for (auto& a : rule.actions) {
        if (a.type == "command") {
            // Prüfen ob ssh_host gesetzt ist
            if (!ssh_host.empty()) {
                // Remote ausführen
                remoteCommand(ssh_host, a.value);
            } else {
                // Lokal ausführen
                localCommand(a.value);
            }
        }
        if (a.type == "speak") {
            speak(a.value);
        }
    }
}
