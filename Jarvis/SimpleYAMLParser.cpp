#include "SimpleYAMLParser.h"
#include <fstream>
#include <sstream>

// Entfernt Tabulator Steuerzeichen
static std::string trim(std::string s) {
    s.erase(0, s.find_first_not_of(" \t\""));
    s.erase(s.find_last_not_of(" \t\"") + 1);
    return s;
}

// Scannt Inhalt aus energieboxCommands.yaml und
// führt command sowie speak bei einem match aus!
std::vector<Rule> SimpleYAMLParser::parse(const std::string& file) {
    std::ifstream f(file);
    std::string line;
    std::vector<Rule> rules;
    Rule* current = nullptr;
    while (std::getline(f, line)) {
        if (line.find("- name:") != std::string::npos) {
            rules.push_back(Rule());
            current = &rules.back();
            current->name = trim(line.substr(line.find(":") + 1));
        } else if (line.find("tonlaut:") != std::string::npos) {
            std::vector<std::string> group;
            auto a = line.find("[");
            auto b = line.find("]");
            std::string inside = line.substr(a + 1, b - a - 1);
            std::stringstream ss(inside);
            std::string item;
            while (std::getline(ss, item, ',')) {
                group.push_back(trim(item));
            }
            current->conditions.push_back(group);
        } else if (line.find("command:") != std::string::npos) {
            current->actions.push_back({"command", trim(line.substr(line.find(":") + 1))});
        } else if (line.find("speak:") != std::string::npos) {
            current->actions.push_back({"speak", trim(line.substr(line.find(":") + 1))});
        }
    }
    return rules;
}
