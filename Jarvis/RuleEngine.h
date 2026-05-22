#pragma once
#include "Rule.h"
#include <vector>
#include <string>
#include <map>

class RuleEngine {
   public:
    RuleEngine(const std::vector<Rule>& rules, std::map<std::string, std::string> config);
    void process(const std::string& input);

   private:
    std::vector<Rule> rules;
    std::string ssh_host;
    bool match(const Rule& rule, const std::string& input);
    void execute(const Rule& rule, const std::string& input);
};
