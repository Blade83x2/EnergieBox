#ifndef INIREADER_H
#define INIREADER_H
#include <fstream>
#include <map>
#include <string>

class IniReader {
   public:
    explicit IniReader(const std::string &path);
    std::string get(const std::string &sectionKey, const std::string &defaultVal = "") const;

   private:
    std::map<std::string, std::string> data_;
};
#endif
