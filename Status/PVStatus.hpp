#pragma once
#include "StatusBlock.hpp"

class PVStatus : public StatusBlock {
    float voltage = 0.0f;
    float current = 0.0f;
    float power = 0.0f;
    float generatedToday = 0.0f;

   public:
    PVStatus& update() override;
    void draw() const override;
};
