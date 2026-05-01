#pragma once
#include "Rule.h"
#include <vector>
#include <string>

class SimpleYAMLParser {
   public:
    std::vector<Rule> parse(const std::string& file);
};
