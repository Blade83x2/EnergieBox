#pragma once
#include <string>
#include <vector>

struct Action {
    std::string type;
    std::string value;
};

struct Rule {
    std::string name;
    std::vector<std::vector<std::string>> conditions;
    std::vector<Action> actions;
};
