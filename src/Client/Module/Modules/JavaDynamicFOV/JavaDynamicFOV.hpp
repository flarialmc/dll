#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Core/Options.hpp"
#include "../Zoom/Zoom.hpp"

class JavaDynamicFOV : public Module {
private:
    static inline float currentFOVVal = 0.0f;
    static float inline realFov = 70.0f;

    static inline float animDisableTreshold = 0.2;

    static inline bool firstTime = true;

public:
    static inline bool animationFinished = true;
    JavaDynamicFOV() : Module("Java Dynamic FOV", "Enhances dynamic FOV in bedrock.", IDR_MAGNIFY_PNG, "C") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, FOVEvent, &JavaDynamicFOV::onGetFOV)
        auto dynamicFov = Options::getOption("fov.toggle");
        if(dynamicFov != nullptr) dynamicFov->setvalue(false);
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, FOVEvent, &JavaDynamicFOV::onGetFOV)
        auto dynamicFov = Options::getOption("fov.toggle");
        if(dynamicFov != nullptr) dynamicFov->setvalue(true); // TODO: make it set to what was before enable
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("fov_target") == nullptr) settings.addSetting("fov_target", 120.0f);
        if (settings.getSettingByName<float>("anim") == nullptr) settings.addSetting("anim", 0.40f);
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
        this->addSlider("Altered FOV target", "Target FOV when sprinting.", this->settings.getSettingByName<float>("fov_target")->value, 175, 0,false);
        this->addSlider("Animation Speed", "Speed of FOV change.", this->settings.getSettingByName<float>("anim")->value, 0.60);
        

        FlarialGUI::UnsetScrollView();
        this->resetPadding();

    }

    void onGetFOV(FOVEvent &event) {
        auto fov = event.getFOV();
        if(fov == 70) return;
        realFov = fov;

        if (ModuleManager::getModule("Zoom").get()->active || !Zoom::animationFinished)
        {
            return;
        }
        if(firstTime){ // so that it doesn't unzoom on module load
            currentFOVVal = fov;
            firstTime = false;
            return;
        }

        auto player = SDK::clientInstance->getLocalPlayer();
        if(!player) return;

        auto fovTarget = this->settings.getSettingByName<float>("fov_target")->value;

        float animspeed = this->settings.getSettingByName<float>("anim")->value;
        bool disableanim = false;
        bool alwaysanim = false;

        if (player->getActorFlag(ActorFlags::FLAG_SPRINTING)) {
            Zoom::jdfAnimationFinished = false;
            if (fov > 180) {
                currentFOVVal = disableanim ? fov + fovTarget : std::lerp(currentFOVVal, fov + fovTarget, animspeed * FlarialGUI::frameFactor);
            } else {
                currentFOVVal = disableanim ? fovTarget : std::lerp(currentFOVVal, fovTarget, animspeed * FlarialGUI::frameFactor);
            }
        } else {
            if ((!Zoom::jdfAnimationFinished || alwaysanim) && !disableanim) {
                // Only lerp if animation hasn't finished
                currentFOVVal = std::lerp(currentFOVVal, fov, animspeed * FlarialGUI::frameFactor);
                if (currentFOVVal == fovTarget || std::abs(fov - currentFOVVal) < animspeed + animDisableTreshold) { // when fov changes due to sprinting animation used to play
                    // Set animationFinished to true only when reaching original fov
                    Zoom::jdfAnimationFinished = true;
                }
            } else {
                currentFOVVal = fov;
            }
        }

        event.setFOV(currentFOVVal);
    }
};
