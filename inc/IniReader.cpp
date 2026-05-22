#include "IniReader.h"
#include <iostream>

IniReader::IniReader(const std::string &path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Fehler beim Öffnen der INI: " << path << std::endl;
        return;
    }

    std::string line;
    std::string currentSection;
    while (std::getline(file, line)) {
        // führende Leerzeichen entfernen
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        // trailing Leerzeichen entfernen
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        // Kommentare / leere Zeilen überspringen
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;
        // Neue Sektion
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        // key=value parsen
        auto eqpos = line.find('=');
        if (eqpos == std::string::npos) continue;
        std::string key = line.substr(0, eqpos);
        std::string value = line.substr(eqpos + 1);
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        data_[currentSection + "/" + key] = value;
    }
}

std::string IniReader::get(const std::string &sectionKey, const std::string &defaultVal) const {
    auto it = data_.find(sectionKey);
    if (it != data_.end()) return it->second;
    return defaultVal;
}
