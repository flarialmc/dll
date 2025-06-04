#include "Zoom.hpp"

Zoom::Zoom(): Module("Zoom", "Allows you to see distant places.", IDR_MAGNIFY_PNG, "C")
{
    Module::setup();
}

void Zoom::onEnable()
{
    Listen(this, FOVEvent, &Zoom::onGetFOV)
    Listen(this, SensitivityEvent, &Zoom::onGetSensitivity)
    Listen(this, MouseEvent, &Zoom::onMouse)
    Listen(this, KeyEvent, &Zoom::onKey)
    Listen(this, SetTopScreenNameEvent, &Zoom::onSetTopScreenName)
    Module::onEnable();
}

void Zoom::onDisable()
{
    Deafen(this, FOVEvent, &Zoom::onGetFOV)
    Deafen(this, SensitivityEvent, &Zoom::onGetSensitivity)
    Deafen(this, MouseEvent, &Zoom::onMouse)
    Deafen(this, KeyEvent, &Zoom::onKey)
    Deafen(this, SetTopScreenNameEvent, &Zoom::onSetTopScreenName)
    Module::onDisable();
}

void Zoom::defaultConfig()
{
    setDef("enabled", true);
    getKeybind();
    Module::defaultConfig("core");
    setDef("alwaysanim", false);
    setDef("SaveModifier", true);
    setDef("hidehand", true);
    setDef("hidemodules", false);
    setDef("UseScroll", true);
    setDef("lowsens", true);
    setDef("toggleZoom", false);
    //if (settings.getSettingByName<bool>("hidehud") == nullptr) settings.addSetting("hidehud", false);
    setDef("modifier", 10.0f);
    setDef("anim", 0.20f);
    setDef("disableanim", false);
}

void Zoom::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Main");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);
    addToggle("Use Scroll", "Allows to adjust zoom with scroll wheel.", "UseScroll");
    addToggle("Toggle Zoom", "No need to hold the keybind to zoom.", "toggleZoom");
    addSlider("Modifier", "How much to Zoom each time you scroll.", "modifier", 30, 0, false);
    addToggle("Disable Animation", "The animation when you zoom", "disableanim");
    addSlider("Animation Speed", "Speed at which scroll zoom acts.", "anim", 0.40);
    addToggle("Save Modifier", "Saves the last zoom amount.", "SaveModifier");
    addToggle("Hide Hand", "Hide hand when zooming.", "hidehand");
    addToggle("Hide Modules", "Hides other modules when zooming.", "hidemodules");
    addToggle("Always Animate", "Smooth zoom animation while sprinting.", "alwaysanim");
    addToggle("Low Sensitivity", "Lower sensitivity when in zoom.", "lowsens");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void Zoom::onGetFOV(FOVEvent& event)
{
    auto fov = event.getFOV();
    if (fov == 70) return;

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player) return;

    if (ModuleManager::getModule("Java Dynamic FOV").get()->isEnabled()) {
        if (player->getActorFlag(ActorFlags::FLAG_SPRINTING)) {
            fov = ModuleManager::getModule("Java Dynamic FOV").get()->getOps<float>("fov_target");
        }
    }

    if (fisrtTime) { // so that it doesn't unzoom on module load
        currentZoomVal = fov;
        fisrtTime = false;
        return;
    }

    float animspeed = getOps<float>("anim");
    bool disableanim = getOps<bool>("disableanim");
    bool alwaysanim = getOps<bool>("alwaysanim");

    if (this->active) {
        animationFinished = false;
        if (fov > 180) {
            currentZoomVal = disableanim ? fov + zoomValue : std::lerp(currentZoomVal, fov + zoomValue, animspeed * FlarialGUI::frameFactor);
        }
        else {
            currentZoomVal = disableanim ? zoomValue : std::lerp(currentZoomVal, zoomValue, animspeed * FlarialGUI::frameFactor);
        }
    }
    else {
        if ((!animationFinished || alwaysanim) && !disableanim) {
            // Only lerp if animation hasn't finished
            if (!jdfAnimationFinished)
            {
                jdfAnimationFinished = true;
            }
            currentZoomVal = std::lerp(currentZoomVal, fov, animspeed * FlarialGUI::frameFactor);
            if (currentZoomVal == zoomValue || std::abs(fov - currentZoomVal) < animspeed + unzoomAnimDisableTreshold) { // when fov changes due to sprinting animation used to play
                // Set animationFinished to true only when reaching original fov
                animationFinished = true;
            }
        }
        else {
            // Once animation finished, set current zoom to fov
            currentZoomVal = fov;
        }
    }

    event.setFOV(currentZoomVal);
}

void Zoom::onGetSensitivity(SensitivityEvent& event)
{
    if (this->active) {
        if (!saved) {
            saved = true;
            currentSensitivity = event.getSensitivity();
        }
        // TODO: smoothstep
        if (getOps<bool>("lowsens")) event.setSensitivity(currentSensitivity - (currentSensitivity * (((realFov - (zoomValue - 1)) / realFov) / 1.0f)));
    }
    else if (saved) {
        saved = false;
    }
}

void Zoom::onMouse(MouseEvent& event)
{
    if (SDK::getCurrentScreen() == "hud_screen" || SDK::getCurrentScreen() == "f1_screen" || SDK::getCurrentScreen() == "zoom_screen")
        if (this->active) {
            //todo make it so that modules work together
            auto fovchanger = ModuleManager::getModule("FOV Changer");
            auto upsidedown = ModuleManager::getModule("Upside Down");
            if (getOps<bool>("UseScroll") == true) {
                if (event.getAction() == MouseAction::ScrollUp) {
                    if ((fovchanger != nullptr &&
                            fovchanger->getOps<float>("fovvalue") > 180) ||
                        (upsidedown != nullptr && upsidedown->isEnabled()))
                        zoomValue += getOps<float>("modifier");
                    else zoomValue -= getOps<float>("modifier");
                }
                if (event.getAction() != MouseAction::ScrollUp && event.getButton() == MouseButton::Scroll) {
                    if ((fovchanger != nullptr &&
                            fovchanger->getOps<float>("fovvalue") > 180) ||
                        (upsidedown != nullptr && upsidedown->isEnabled()))
                        zoomValue -= getOps<float>("modifier");
                    else zoomValue += getOps<float>("modifier");
                }

                if (zoomValue < 1) zoomValue = 1;
                else if (zoomValue > realFov) zoomValue = realFov;

                if (event.getAction() == MouseAction::ScrollUp ||
                    event.getAction() != MouseAction::ScrollUp && event.getButton() == MouseButton::Scroll) {
                    event.setButton(MouseButton::None);
                    event.setAction(MouseAction::Release);


                }
            }
        }
}

void Zoom::onKey(KeyEvent& event)
{

    if (this->isKeybind(event.keys) &&
        this->isKeyPartOfKeybind(event.key) &&
        (getOps<bool>("toggleZoom") ? event.getAction() == ActionType::Pressed : true)
        && SDK::getCurrentScreen() == "hud_screen") {
        keybindActions[0]({});
        if (!getOps<bool>("SaveModifier")) zoomValue = 30.0f;
    } else if (getOps<bool>("toggleZoom") ? (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) : !this->isKeybind(event.keys) && (getOps<bool>("toggleZoom") ? event.getAction() == ActionType::Pressed : true)) this->active = false;
}

void Zoom::onSetTopScreenName(SetTopScreenNameEvent& event)
{
    auto hideHand = Options::getOption("hidehand");
    auto hideHud = Options::getOption("hidehud");
    if (this->active) {
        if (getOps<bool>("hidemodules")) {
            event.setCustomLayer("zoom_screen");
        }
        //            if (getOps<bool>("hidehud")) {
        //                event.setCustomLayer("zoom_screen");
        //                if (hideHand != nullptr) hideHand->setvalue(true);
        //                if (hideHud != nullptr) hideHud->setvalue(true);
        //            }
        if (getOps<bool>("hidehand")) {
            if (hideHand != nullptr) hideHand->setvalue(true);
        }
    }
    else {
        if (getOps<bool>("hidehand")) {
            if (hideHand != nullptr) hideHand->setvalue(false);
        }

        //            if (getOps<bool>("hidehud")) { // TODO: there is a bug where it wont allow manual F1
        //                if (hideHud != nullptr) hideHud->setvalue(false);
        //            }

        if (hideHud != nullptr && hideHud->getvalue()) {
            event.setCustomLayer("f1_screen");
        }
    }
}
