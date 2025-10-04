#pragma once

#include "../Module.hpp"
#include "Events/Game/FOVEvent.hpp"
#include "Events/Game/PerspectiveEvent.hpp"
#include "Events/Game/RenderItemInHandEvent.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"

class ViewModel : public Module {

private:

	bool thirdperson = false;

public:

	ViewModel(): Module("View Model", "Allows you to modify how item in hand looks.",
		IDR_EYE_PNG, "C", false, {"item", "size"}) {

	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onGetPerspective(PerspectiveEvent& event);

	void onGetFOV(FOVEvent& event);

	void onRenderItemInHand(RenderItemInHandEvent& event);

	glm::mat4x4 OriginalMatrix;
	bool Matrixed = false;
};
