#pragma once

#include "../Module.hpp"

class ViewModel : public Module {
private:
	bool thirdperson = false;
public:
	ViewModel() : Module("View Model", "Allows you to modify how item in hand looks.", IDR_EYE_PNG, "C") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, FOVEvent, &ViewModel::onGetFOV)
			Listen(this, RenderItemInHandEvent, &ViewModel::onRenderItemInHand)
			Listen(this, PerspectiveEvent, &ViewModel::onGetPerspective)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, FOVEvent, &ViewModel::onGetFOV)
			Deafen(this, RenderItemInHandEvent, &ViewModel::onRenderItemInHand)
			Deafen(this, PerspectiveEvent, &ViewModel::onGetPerspective)
			if (Matrixed) {
				auto& matrix = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;
				matrix = OriginalMatrix;
			}

		Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig("core");
		setDef("itemfov", 70.0f);
		setDef("thirdperson", false);
		setDef("posx", 4.0f);
		setDef("posy", 4.0f);
		setDef("posz", 4.0f);
		setDef("rotangle", 0.0f);
		setDef("rotx", 0.0f);
		setDef("roty", 0.0f);
		setDef("rotz", 0.0f);
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		addHeader("View Model");
		addToggle("Third Person", "Transforms the item in third person perspective", getOps<bool>("thirdperson"));
		addSlider("Item FOV", "Changes the FOV appearance of the item.", getOps<float>("itemfov"), 180);

		addSlider("Position X", "Changes the position in the X axis", getOps<float>("posx"), 12);
		addSlider("Position Y", "Changes the position in the Y axis", getOps<float>("posy"), 12);
		addSlider("Position Z", "Changes the position in the Z axis", getOps<float>("posz"), 12);

		addSlider("Rotation Angle", "Changes the rotation angle of the item", getOps<float>("rotangle"), 360);
		addSlider("Rotation X", "Changes the rotation in the X axis", getOps<float>("rotx"), 360);
		addSlider("Rotation Y", "Changes the rotation in the Y axis", getOps<float>("roty"), 360);
		addSlider("Rotation Z", "Changes the rotation in the Z axis", getOps<float>("rotz"), 360);

		FlarialGUI::UnsetScrollView();
		resetPadding();
	}

	void onGetPerspective(PerspectiveEvent& event) {
		if (event.getPerspective() == Perspective::FirstPerson) thirdperson = false;
		else thirdperson = true;
	}

	void onGetFOV(FOVEvent& event) {
		auto fov = event.getFOV();
		if (fov != 70) return;

		event.setFOV(getOps<float>("itemfov"));
	}

	void onRenderItemInHand(RenderItemInHandEvent& event) {
		if (thirdperson && getOps<bool>("thirdperson") || !thirdperson) {
			auto& matrix = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;
			if (!Matrixed) OriginalMatrix = matrix;

			auto posx = getOps<float>("posx");
			auto posy = getOps<float>("posy");
			auto posz = getOps<float>("posz");

			auto rotx = getOps<float>("rotx");
			auto roty = getOps<float>("roty");
			auto rotz = getOps<float>("rotz");

			auto rotAngle = getOps<float>("rotangle");

			matrix = glm::translate<float>(matrix, glm::vec3(posx - 4, posy - 4, posz - 4));
			matrix = glm::rotate<float>(matrix, glm::radians(rotAngle), glm::vec3(rotx, roty, rotz));
		}
	}

	glm::mat4x4 OriginalMatrix;
	bool Matrixed = false;
};
