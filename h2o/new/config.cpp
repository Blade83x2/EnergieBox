#include "config.h"
#include <fstream>   // Für std::ifstream, std::ofstream
#include <iostream>  // Für std::cerr, std::cout
#include <sstream>   // Falls verwendet
#include <string>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <cctype>

Config::Config(const std::string& path) : filepath(path) {
    load();
}

void Config::load() {
    data.clear();
    std::ifstream file(filepath);
    if (!file.is_open()) return;
    std::string line, currentSection;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '#') continue;

        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
        } else {
            auto delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                key.erase(key.find_last_not_of(" \t") + 1);
                key.erase(0, key.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                data[currentSection][key] = value;
            }
        }
    }
}

void Config::save() const {
    std::ofstream file(filepath);
    for (const auto& section : data) {
        file << "[" << section.first << "]\n";
        for (const auto& kv : section.second) {
            file << kv.first << " = " << kv.second << "\n";
        }
        file << "\n";
    }
}

std::pair<std::string, std::string> Config::splitKey(const std::string& key) {
    auto pos = key.find('/');
    if (pos == std::string::npos) throw std::invalid_argument("Invalid key format, use section/key");
    return {key.substr(0, pos), key.substr(pos + 1)};
}

// Entfernt führende und folgende Leerzeichen aus einem String
std::string Config::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";  // String besteht nur aus Leerzeichen
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Wandelt einen String in Kleinbuchstaben um
std::string Config::toLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    return str;
}
