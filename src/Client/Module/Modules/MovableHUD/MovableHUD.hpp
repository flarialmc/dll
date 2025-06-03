#pragma once

#include "../Module.hpp"

// TODO: combine into 1

struct HUDElement {
	std::string mcName;
	Vec2<float> currentPos{ -120.0f, -120.0f };;
	bool* enabled{};
	static inline Vec2<float> originalPos = Vec2<float>{ 0.0f, 0.0f };
	Vec2<float> currentSize = Vec2<float>{ 0.0f, 0.0f };
};

class MovableHUD : public Module {
private:
	std::array<HUDElement, 6> elements{};
	Vec2<float> currentPos{ -120.0f, -120.0f };;
	bool enabled = false;
	static inline Vec2<float> originalPos = Vec2<float>{ 0.0f, 0.0f };
	Vec2<float> currentSize = Vec2<float>{ 0.0f, 0.0f };
public:
	MovableHUD();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;
};