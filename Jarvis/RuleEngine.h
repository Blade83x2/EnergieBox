#pragma once
#include "Rule.h"
#include <vector>
#include <string>

class RuleEngine {
   public:
    RuleEngine(const std::vector<Rule>& rules, const std::string& ssh_host);
    void process(const std::string& input);

   private:
    std::vector<Rule> rules;
    std::string ssh_host;
    bool match(const Rule& rule, const std::string& input);
    void execute(const Rule& rule, const std::string& input);
};
