#ifndef CONFIG_H
#define CONFIG_H
#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <stdexcept>
#include <type_traits>

class Config {
   public:
    explicit Config(const std::string &filepath);

    // get ohne Default - wirft Ausnahme bei Key nicht gefunden
    template <typename T>
    T get(const std::string &key) const;

    // get mit Default-Wert
    template <typename T>
    T get(const std::string &key, const T &defaultValue) const;

    template <typename T>
    void set(const std::string &key, const T &value);

   private:
    std::string filepath;
    std::unordered_map<std::string, std::map<std::string, std::string>> data;

    void load();
    void save() const;

    static std::string toLower(std::string str);
    static std::pair<std::string, std::string> splitKey(const std::string &key);
    static std::string trim(const std::string &s);

    template <typename T>
    static T convertTo(const std::string &value);

    template <typename T>
    static std::string convertFrom(const T &value);
};

// --- Template-Implementierungen ---

template <typename T>
T Config::get(const std::string &key) const {
    auto [section, name] = splitKey(key);
    auto secIt = data.find(section);
    if (secIt != data.end()) {
        auto varIt = secIt->second.find(name);
        if (varIt != secIt->second.end()) {
            return convertTo<T>(varIt->second);
        }
    }
    throw std::runtime_error("Key not found: " + key);
}

template <typename T>
T Config::get(const std::string &key, const T &defaultValue) const {
    auto [section, name] = splitKey(key);
    auto secIt = data.find(section);
    if (secIt != data.end()) {
        auto varIt = secIt->second.find(name);
        if (varIt != secIt->second.end()) {
            try {
                return convertTo<T>(varIt->second);
            } catch (...) {
                // Falls Konvertierung fehlschlägt, Default zurückgeben
                return defaultValue;
            }
        }
    }
    return defaultValue;
}

template <typename T>
void Config::set(const std::string &key, const T &value) {
    auto [section, name] = splitKey(key);
    data[section][name] = convertFrom<T>(value);
    save();
}

// Beispiel-Konvertierungen
template <typename T>
T Config::convertTo(const std::string &value) {
    if constexpr (std::is_same<T, int>::value) {
        return std::stoi(value);
    } else if constexpr (std::is_same<T, float>::value) {
        return std::stof(value);
    } else if constexpr (std::is_same<T, double>::value) {
        return std::stod(value);
    } else if constexpr (std::is_same<T, bool>::value) {
        return (value == "1" || value == "true" || value == "True");
    } else if constexpr (std::is_same<T, std::string>::value) {
        return value;
    } else {
        static_assert(!sizeof(T), "Unsupported type for Config::get");
    }
}

template <typename T>
std::string Config::convertFrom(const T &value) {
    if constexpr (std::is_same<T, bool>::value) {
        return value ? "1" : "0";
    } else {
        return std::to_string(value);
    }
}

template <>
inline std::string Config::convertFrom<std::string>(const std::string &value) {
    return value;
}

#endif  // CONFIG_H
