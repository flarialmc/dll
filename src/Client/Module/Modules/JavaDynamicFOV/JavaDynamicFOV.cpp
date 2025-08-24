#include "JavaDynamicFOV.hpp"

void JavaDynamicFOV::onEnable() {
    Listen(this, FOVEvent, &JavaDynamicFOV::onGetFOV)
    auto dynamicFov = Options::getOption("fov.toggle");
    if (dynamicFov != nullptr) dynamicFov->setvalue(false);
    Module::onEnable();
}

void JavaDynamicFOV::onDisable() {
    Deafen(this, FOVEvent, &JavaDynamicFOV::onGetFOV)
    auto dynamicFov = Options::getOption("fov.toggle");
    if (dynamicFov != nullptr) dynamicFov->setvalue(true); // TODO: make it set to what was before enable
    Module::onDisable();
}

void JavaDynamicFOV::defaultConfig() {
    Module::defaultConfig("core");
    setDef("fov_target", 120.0f);
    setDef("anim", 0.40f);
    
}

void JavaDynamicFOV::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Java Dynamic FOV");
    addSlider("Altered FOV target", "Target FOV when sprinting.", "fov_target", 175, 0, false);
    addSlider("Animation Speed", "Speed of FOV change.", "anim", 0.60);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void JavaDynamicFOV::onGetFOV(FOVEvent &event) {
    if (!this->isEnabled()) return;
    auto fov = event.getFOV();
    if (fov == 70) return;
    realFov = fov;

    if (ModuleManager::getModule("Zoom").get()->active || !Zoom::animationFinished)
    {
        return;
    }
    if (firstTime) { // so that it doesn't unzoom on module load
        currentFOVVal = fov;
        firstTime = false;
        return;
    }

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player) return;

    auto fovTarget = getOps<float>("fov_target");

    float animspeed = getOps<float>("anim");
    bool disableanim = false;
    bool alwaysanim = false;

    if (player->getActorFlag(ActorFlags::FLAG_SPRINTING)) {
        Zoom::jdfAnimationFinished = false;
        if (fov > 180) {
            currentFOVVal = disableanim ? fov + fovTarget : std::lerp(currentFOVVal, fov + fovTarget, animspeed * FlarialGUI::frameFactor);
        }
        else {
            currentFOVVal = disableanim ? fovTarget : std::lerp(currentFOVVal, fovTarget, animspeed * FlarialGUI::frameFactor);
        }
    }
    else {
        if ((!Zoom::jdfAnimationFinished || alwaysanim) && !disableanim) {
            // Only lerp if animation hasn't finished
            currentFOVVal = std::lerp(currentFOVVal, fov, animspeed * FlarialGUI::frameFactor);
            if (currentFOVVal == fovTarget || std::abs(fov - currentFOVVal) < animspeed + animDisableTreshold) { // when fov changes due to sprinting animation used to play
                // Set animationFinished to true only when reaching original fov
                Zoom::jdfAnimationFinished = true;
            }
        }
        else {
            currentFOVVal = fov;
        }
    }

    event.setFOV(currentFOVVal);
}
