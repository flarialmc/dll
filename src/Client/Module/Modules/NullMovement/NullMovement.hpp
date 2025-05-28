#pragma once

#include "../Module.hpp"


class NullMovement : public Module {
	OptionsParser parser;
	int lastKey;
	int lastLastKey;
	std::array<bool, 256> lastKeys;
	ActionType lastAction;
	std::vector<int> movementKeyStack;
	bool unpresser = false;

public:
	NullMovement() : Module("Null Movement", "Only registers the latest movement key.",
		IDR_SPEED_PNG, "") {

		Module::setup();
	};

	void onEnable() override {
		parser.parseOptionsFile();
		Listen(this, KeyEvent, &NullMovement::onKey)

			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, KeyEvent, &NullMovement::onKey)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<bool>("horizontal") == nullptr) settings.addSetting("horizontal", true);
		if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);
	}


	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Misc");
		this->addToggle("Vertical Nulling", "W & S keys", settings.getSettingByName<bool>("vertical")->value);
		this->addToggle("Horizontal Nulling", "A & D keys", settings.getSettingByName<bool>("horizontal")->value);

		FlarialGUI::UnsetScrollView();

		this->resetPadding();

	}



	void onKey(KeyEvent& event) {
		lastKey = event.getKey();
		lastKeys = event.keys;
		lastAction = event.getAction();


		static int forwardKey = safe_stoi(parser.options["keyboard_type_0_key.forward"]);
		static int backwardKey = safe_stoi(parser.options["keyboard_type_0_key.back"]);
		static int leftKey = safe_stoi(parser.options["keyboard_type_0_key.left"]);
		static int rightKey = safe_stoi(parser.options["keyboard_type_0_key.right"]);


		bool isMovementKey = (lastKey == forwardKey ||
			lastKey == backwardKey ||
			lastKey == leftKey ||
			lastKey == rightKey);

		if (lastAction == ActionType::Pressed && isMovementKey) unpresser = true;

		if (!isMovementKey)
			return;
		if (lastAction == ActionType::Pressed) {
			auto it = std::find(movementKeyStack.begin(), movementKeyStack.end(), lastKey);
			if (it != movementKeyStack.end()) {
				movementKeyStack.erase(it);
			}
			movementKeyStack.push_back(lastKey);
		}
		else if (lastAction == ActionType::Released) {
			movementKeyStack.erase(
				std::remove(movementKeyStack.begin(), movementKeyStack.end(), lastKey),
				movementKeyStack.end()
			);
		}

		if (!movementKeyStack.empty()) {
			int lastKey = movementKeyStack.back();
			if (lastKey == forwardKey && settings.getSettingByName<bool>("vertical")->value)  KeyHook::funcOriginal(backwardKey, 0);
			else if (lastKey == backwardKey && settings.getSettingByName<bool>("vertical")->value) KeyHook::funcOriginal(forwardKey, 0);
			else if (lastKey == leftKey && settings.getSettingByName<bool>("horizontal")->value) KeyHook::funcOriginal(rightKey, 0);
			else if (lastKey == rightKey && settings.getSettingByName<bool>("horizontal")->value) KeyHook::funcOriginal(leftKey, 0);
		}
	}

	int safe_stoi(const std::string& str) {
		char* end;
		long value = std::strtol(str.c_str(), &end, 10);
		if (*end != '\0') {
			throw std::invalid_argument("Invalid integer: " + str);
		}
		return static_cast<int>(value);
	}
};