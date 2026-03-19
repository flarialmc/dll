#pragma once

#include "../HUDModule.hpp"
#include "../../../../Assets/Assets.hpp"

class PitchDisplay : public HUDModule {
private:
	float pitch = 0.f;

public:
	PitchDisplay() : HUDModule(12, "Pitch Display", "Shows your pitch.",
		IDR_SKULL_PNG, "") {
	}

protected:
	std::string getDisplayValue() override;
	void customConfig() override;
	void customSettings() override;
};
