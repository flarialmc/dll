#pragma once

#include "../Module.hpp"
#include "Events/Game/PerspectiveEvent.hpp"

class SnapLook : public Module {
public:
	SnapLook();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	// TODO: make it togglable
	void onKey(KeyEvent& event);;

	void onGetViewPerspective(PerspectiveEvent& event) const;
};
