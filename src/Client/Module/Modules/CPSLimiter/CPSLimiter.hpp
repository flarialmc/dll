#pragma once

#include "../Module.hpp"

class CPSLimiter : public Module {

public:
	CPSLimiter() : Module("CPS Limiter", "Limit how many clicks you can\nregister per second.",
		IDR_STOP_PNG, "", false, {"dcprevent", "dc prevent"}) {
		//this->setup();
	};

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;
};

