#pragma once
#include "StatusBlock.hpp"

class BatteryStatus : public StatusBlock
{
	float soc = 0.0f;
	float voltage = 0.0f;

  public:
	bool update() override;
	void draw() const override;
};
