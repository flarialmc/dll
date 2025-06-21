#pragma once

#include "../Module.hpp"
#include "Events/Game/FOVEvent.hpp"
#include "Events/Game/PerspectiveEvent.hpp"
#include "Events/Game/RenderItemInHandEvent.hpp"

class ViewModel : public Module {
private:
	bool thirdperson = false;
public:
	ViewModel();;

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
