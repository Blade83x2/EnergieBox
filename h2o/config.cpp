#include "config.h"
#include <fstream>   // Für std::ifstream, std::ofstream
#include <iostream>  // Für std::cerr, std::cout
#include <sstream>   // Falls verwendet
#include <string>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <type_traits>

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

// Spezialisierungen

template <>
int Config::get<int>(const std::string& key, int defaultValue) const {
    auto [section, name] = splitKey(key);
    try {
        return std::stoi(data.at(section).at(name));
    } catch (...) {
        return defaultValue;
    }
}

template <>
bool Config::get<bool>(const std::string& key, bool defaultValue) const {
    auto [section, name] = splitKey(key);
    try {
        return data.at(section).at(name) == "1";
    } catch (...) {
        return defaultValue;
    }
}

template <>
float Config::get<float>(const std::string& key, float defaultValue) const {
    auto [section, name] = splitKey(key);
    try {
        return std::stof(data.at(section).at(name));
    } catch (...) {
        return defaultValue;
    }
}

template <>
std::string Config::get<std::string>(const std::string& key, std::string defaultValue) const {
    auto [section, name] = splitKey(key);
    try {
        return data.at(section).at(name);
    } catch (...) {
        return defaultValue;
    }
}

template <>
void Config::set<int>(const std::string& key, const int& value) {
    auto [section, name] = splitKey(key);
    data[section][name] = std::to_string(value);
    save();
}

template <>
void Config::set<bool>(const std::string& key, const bool& value) {
    auto [section, name] = splitKey(key);
    data[section][name] = value ? "1" : "0";
    save();
}

template <>
void Config::set<float>(const std::string& key, const float& value) {
    auto [section, name] = splitKey(key);
    data[section][name] = std::to_string(value);
    save();
}

template <>
void Config::set<std::string>(const std::string& key, const std::string& value) {
    auto [section, name] = splitKey(key);
    data[section][name] = value;
    save();
}
