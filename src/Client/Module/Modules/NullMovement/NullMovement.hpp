#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "SDK/Client/Options/OptionsParser.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"


class NullMovement : public Module {
	OptionsParser parser;
	int lastKey;
	int lastLastKey;
	std::array<bool, 256> lastKeys;
	ActionType lastAction;
	std::vector<int> movementKeyStack;
	bool unpresser = false;

public:

	NullMovement(): Module("Null Movement", "Only registers the latest movement key.",
		IDR_NULL_PNG, "") {

	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;


	void settingsRender(float settingsOffset) override;

	void onTick(TickEvent& event);
	void onKey(KeyEvent& event);

	static int safe_stoi(const std::string& str);
};
