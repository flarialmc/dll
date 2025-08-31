#pragma once

#include "../Module.hpp"
#include "Events/Game/PerspectiveEvent.hpp"

class SnapLook : public Module {

private:

	std::unordered_map<std::string, Perspective> perspectiveMap = {
		{"Third Person Front", Perspective::ThirdPersonFront},
		{"Third Person Back", Perspective::ThirdPersonBack},
		{"First Person", Perspective::FirstPerson}
	};

public:
	SnapLook() : Module("SnapLook", "Quickly look behind you.",
		IDR_EYE_PNG, "V") {

	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onKey(KeyEvent& event);

	void onMouse(MouseEvent& event);

	void onGetViewPerspective(PerspectiveEvent& event);
};
