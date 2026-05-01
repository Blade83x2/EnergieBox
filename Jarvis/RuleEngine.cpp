#include "RuleEngine.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cctype>

extern void speak(const std::string&);
extern int exec(const std::string&);
extern void remoteCommand(const std::string&, const std::string&);

RuleEngine::RuleEngine(const std::vector<Rule>& r, const std::string& host) : rules(r), ssh_host(host) {}

// Durchläuft alle Gruppen aus yaml Datei
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

// Vergleicht Eingabe mit yaml Datei
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
            remoteCommand(ssh_host, a.value);
        }
        if (a.type == "speak") {
            speak(a.value);
        }
    }
}
