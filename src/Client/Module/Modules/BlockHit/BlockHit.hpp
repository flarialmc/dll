#pragma once

#include "../Module.hpp"
#include "../CPS/CPSCounter.hpp"
#include "../../../Events/Game/RenderItemInHandEvent.hpp"

class BlockHit : public Module {
public:
	Perspective perspective;

	BlockHit() : Module("Block Hit", "Sword Blocking Animation like Java (visual only)",
		IDR_SWORD_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Module::onEnable();
		Listen(this, RenderItemInHandEvent, &BlockHit::onItemInHandRender);
		Listen(this, PerspectiveEvent, &BlockHit::onPerspectiveChange);
	}

	void onDisable() override {
		Deafen(this, RenderItemInHandEvent, &BlockHit::onItemInHandRender);
		Deafen(this, PerspectiveEvent, &BlockHit::onPerspectiveChange);
		Module::onDisable();
	}

	void onPerspectiveChange(PerspectiveEvent& event) {
		this->perspective = event.getPerspective();
	}

	void onItemInHandRender(RenderItemInHandEvent& event) {

		auto& matrix = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;
		auto itemStack = event.itemStack;

		if (itemStack->item.get() != nullptr && CPSCounter::GetRightHeld() && itemStack->getItem()->name.contains("sword")) {

			switch (perspective) {
			case Perspective::FirstPerson:

				matrix = glm::translate<float>(matrix, glm::vec3(-0.5f, 0.2f, 0.0f));
				matrix = glm::rotate<float>(matrix, glm::radians(30.f), glm::vec3(0.f, 1.f, 0.f));
				matrix = glm::rotate<float>(matrix, glm::radians(-80.f), glm::vec3(1.f, 0.f, 0.f));
				matrix = glm::rotate<float>(matrix, glm::radians(60.f), glm::vec3(0.f, 1.f, 0.f));

				break;

			case Perspective::ThirdPersonBack:

				matrix = glm::translate<float>(matrix, glm::vec3(-0.5f, 0.2f, 0.2f));
				matrix = glm::rotate<float>(matrix, glm::radians(105.f), glm::vec3(0.f, 1.f, 0.f));
				matrix = glm::rotate<float>(matrix, glm::radians(-100.f), glm::vec3(1.f, 0.f, 0.f));
				matrix = glm::rotate<float>(matrix, glm::radians(130.f), glm::vec3(0.f, 1.f, 0.f));

				break;

			case Perspective::ThirdPersonFront:

				matrix = glm::translate<float>(matrix, glm::vec3(-0.5f, 0.2f, 0.2f));
				matrix = glm::rotate<float>(matrix, glm::radians(105.f), glm::vec3(0.f, 1.f, 0.f));
				matrix = glm::rotate<float>(matrix, glm::radians(-100.f), glm::vec3(1.f, 0.f, 0.f));
				matrix = glm::rotate<float>(matrix, glm::radians(130.f), glm::vec3(0.f, 1.f, 0.f));

				break;
			}
		}
	}

    void defaultConfig() override {
        Module::defaultConfig();
    }

	void settingsRender(float settingsOffset) override {


		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		FlarialGUI::UnsetScrollView();

		this->resetPadding();
	}
};

