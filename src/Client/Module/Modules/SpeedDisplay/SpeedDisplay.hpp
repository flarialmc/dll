#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"

class SpeedDisplay : public Module {
private:
	Vec3<float> PrevPos{};
	std::string speed;
public:
	SpeedDisplay();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);

	void onTick(TickEvent& event);
};

