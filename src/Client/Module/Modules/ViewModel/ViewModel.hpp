#pragma once

#include "../Module.hpp"


class ViewModel : public Module {
private:
    bool thirdperson = false;
public:
    ViewModel() : Module("ViewModel", "Allows you to modify how item in hand looks.", IDR_EYE_PNG, "C") {
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

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<float>("itemfov") == nullptr) settings.addSetting("itemfov", 70.0f);
        if (settings.getSettingByName<bool>("thirdperson") == nullptr) settings.addSetting("thirdperson", false);

        if (settings.getSettingByName<float>("posx") == nullptr) settings.addSetting("posx", 4.0f);
        if (settings.getSettingByName<float>("posy") == nullptr) settings.addSetting("posy", 4.0f);
        if (settings.getSettingByName<float>("posz") == nullptr) settings.addSetting("posz", 4.0f);

        if (settings.getSettingByName<float>("rotangle") == nullptr) settings.addSetting("rotangle", 0.0f);

        if (settings.getSettingByName<float>("rotx") == nullptr) settings.addSetting("rotx", 0.0f);
        if (settings.getSettingByName<float>("roty") == nullptr) settings.addSetting("roty", 0.0f);
        if (settings.getSettingByName<float>("rotz") == nullptr) settings.addSetting("rotz", 0.0f);
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));


        this->addHeader("Main");
        this->addToggle("Third Person", "Transforms the item in third person perspective", this->settings.getSettingByName<bool>("thirdperson")->value);
        this->addSlider("Item FOV", "Changes the FOV appearance of the item.", this->settings.getSettingByName<float>("itemfov")->value, 180);

        this->addSlider("Position X", "Changes the position in the X axis", this->settings.getSettingByName<float>("posx")->value, 12);
        this->addSlider("Position Y", "Changes the position in the Y axis", this->settings.getSettingByName<float>("posy")->value, 12);
        this->addSlider("Position Z", "Changes the position in the Z axis", this->settings.getSettingByName<float>("posz")->value, 12);

        this->addSlider("Rotation Angle", "Changes the rotation angle of the item", this->settings.getSettingByName<float>("rotangle")->value, 360);
        this->addSlider("Rotation X", "Changes the rotation in the X axis", this->settings.getSettingByName<float>("rotx")->value, 360);
        this->addSlider("Rotation Y", "Changes the rotation in the Y axis", this->settings.getSettingByName<float>("roty")->value, 360);
        this->addSlider("Rotation Z", "Changes the rotation in the Z axis", this->settings.getSettingByName<float>("rotz")->value, 360);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    void onGetPerspective(PerspectiveEvent &event) {
        if (event.getPerspective() == Perspective::FirstPerson) thirdperson = false;
		else thirdperson = true;
	}

    void onGetFOV(FOVEvent &event) {
        auto fov = event.getFOV();
        if (fov != 70) return;

        event.setFOV(this->settings.getSettingByName<float>("itemfov")->value);
    }

    void onRenderItemInHand(RenderItemInHandEvent& event) {
        if (thirdperson && this->settings.getSettingByName<bool>("thirdperson")->value || !thirdperson) {
            auto& matrix = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;
            if (!Matrixed) OriginalMatrix = matrix;

            auto posx = this->settings.getSettingByName<float>("posx")->value;
            auto posy = this->settings.getSettingByName<float>("posy")->value;
            auto posz = this->settings.getSettingByName<float>("posz")->value;

            auto rotx = this->settings.getSettingByName<float>("rotx")->value;
            auto roty = this->settings.getSettingByName<float>("roty")->value;
            auto rotz = this->settings.getSettingByName<float>("rotz")->value;

            auto rotAngle = this->settings.getSettingByName<float>("rotangle")->value;

            matrix = glm::translate<float>(matrix, glm::vec3(posx - 4, posy - 4, posz - 4));
            matrix = glm::rotate<float>(matrix, glm::radians(rotAngle), glm::vec3(rotx, roty, rotz));
        }
    }

    glm::mat4x4 OriginalMatrix;
    bool Matrixed = false;
};
