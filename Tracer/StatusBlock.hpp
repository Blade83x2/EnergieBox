#pragma once
#include <iostream>
#include <cstdio>
#include <cstring>

class StatusBlock {
protected:
    static constexpr int WIDTH_LABEL = 12;
    static constexpr int WIDTH_VALUE = 10;
    static constexpr int WIDTH_BAR   = 22;
    static constexpr int TOTAL_WIDTH = WIDTH_LABEL + WIDTH_VALUE + 1 + WIDTH_BAR;

    float extractValue(const char* line);

    void printBar(float value, float maxValue) const;
    void printFloat(float value, const char* unit, bool color = false, bool rightJustified = true) const;

public:
    static void clearScreen();
    static void printHeader(const char* title);

    virtual bool update() = 0;
    virtual void draw() const = 0;
    virtual ~StatusBlock() = default;
};
