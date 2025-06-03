#pragma once

#include "../Module.hpp"
#include "SDK/Client/Options/OptionsParser.hpp"


class NullMovement : public Module {
	OptionsParser parser;
	int lastKey;
	int lastLastKey;
	std::array<bool, 256> lastKeys;
	ActionType lastAction;
	std::vector<int> movementKeyStack;
	bool unpresser = false;

public:
	NullMovement();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;


	void settingsRender(float settingsOffset) override;


	void onKey(KeyEvent& event);

	static int safe_stoi(const std::string& str);
};
