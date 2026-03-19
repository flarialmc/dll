#include "Zoom.hpp"


void Zoom::onEnable() {
    Listen(this, FOVEvent, &Zoom::onGetFOV)
    Listen(this, RenderEvent, &Zoom::onRender)
    Listen(this, MouseEvent, &Zoom::onMouse)
    Listen(this, KeyEvent, &Zoom::onKey)
    Listen(this, SetTopScreenNameEvent, &Zoom::onSetTopScreenName)
    Listen(this, TurnDeltaEvent, &Zoom::onTurnDeltaEvent)
    Listen(this, SensitivityEvent, &Zoom::onSensitivity)
    Module::onEnable();
}

void Zoom::onDisable() {
    Deafen(this, FOVEvent, &Zoom::onGetFOV)
    Deafen(this, RenderEvent, &Zoom::onRender)
    Deafen(this, MouseEvent, &Zoom::onMouse)
    Deafen(this, KeyEvent, &Zoom::onKey)
    Deafen(this, SetTopScreenNameEvent, &Zoom::onSetTopScreenName)
    Deafen(this, TurnDeltaEvent, &Zoom::onTurnDeltaEvent)
    Deafen(this, SensitivityEvent, &Zoom::onSensitivity)
    Module::onDisable();
}

void Zoom::defaultConfig() {
    setDef("enabled", true);
    setDef("keybind", (std::string) "C");
    getKeybind();
    Module::defaultConfig("core");
    setDef("alwaysanim", false);
    setDef("SaveModifier", true);
    setDef("hidehand", true);
    setDef("hidemodules", false);
    setDef("UseScroll", true);
    setDef("lowsens", true);
    setDef("lowsensStrength", 1.f);
    setDef("toggleZoom", false);
    //if (settings.getSettingByName<bool>("hidehud") == nullptr) settings.addSetting("hidehud", false);
    setDef("modifier", 10.0f);
    setDef("anim", 0.20f);
    setDef("disableanim", false);
    setDef("cinematicMode", false);

    // cinematic camera options
    setDef("smoothing", true);
    setDef("smoothness", 8.f);
    setDef("cinebars", false);
    setDef("cinebar", "000000", 1.f, false);
    setDef("cinebarHeight", 0.2f);
}

void Zoom::settingsRender(float settingsOffset) {
    initSettingsPage();

    addKeybind("Keybind", "", "keybind", true);
    addToggle("Use Scroll", "Allows to adjust zoom with scroll wheel.", "UseScroll");
    addToggle("Toggle Zoom", "No need to hold the keybind to zoom.", "toggleZoom");
    addSlider("Modifier", "How much to Zoom each time you scroll.", "modifier", 30, 0, false);
    addToggle("Disable Animation", "The animation when you zoom", "disableanim");
    addSlider("Animation Speed", "Speed at which scroll zoom acts.", "anim", 0.40);
    addToggle("Save Modifier", "Saves the last zoom amount.", "SaveModifier");
    addToggle("Hide Hand", "Hide hand when zooming.", "hidehand");
    addToggle("Hide Modules", "Hides other modules when zooming.", "hidemodules");
    addToggle("Always Animate", "Smooth zoom animation while sprinting.", "alwaysanim");
    addToggle("Enable Cinematic Camera", "Whether to use cinematic camera on zoom or not", "cinematicMode");

    if (getOps<bool>("cinematicMode")) {
        addHeader("Cinematic Camera");
        // direct copy paste from cinematic camera module
        addToggle("Enable Smoothing", "", "smoothing");
        addConditionalSlider(getOps<bool>("smoothing"), "Smoothing", "", "smoothness", 10.f);
        addToggle("Cinematic Bars", "", "cinebars");
        addConditionalSlider(getOps<bool>("cinebars"), "Cinematic Bar Height", "", "cinebarHeight", 0.8f);
        addConditionalColorPicker(getOps<bool>("cinebars"), "Cinematic Bar Color", "", "cinebar");
    } else {
        addHeader("Low Sensitivity");
        addToggle("Low Sensitivity", "Lower sensitivity when in zoom.", "lowsens");
        addConditionalSlider(getOps<bool>("lowsens"), "Low Sensitivity Strength", "", "lowsensStrength", 1.f, 0.f, false);
    }

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void Zoom::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;

    if (this->active && getOps<bool>("cinematicMode") && getOps<bool>("cinebars") && (SDK::getCurrentScreen() == "hud_screen" || SDK::getCurrentScreen() == "f1_screen" || SDK::getCurrentScreen() == "zoom_screen" || SDK::getCurrentScreen() == "f3_screen")) {
        float barHeight = MC::windowSize.y * getOps<float>("cinebarHeight") / 2.f;

        FlarialGUI::RoundedRect(0, 0, getColor("cinebar"), MC::windowSize.x, barHeight, 0, 0);
        FlarialGUI::RoundedRect(0, MC::windowSize.y - barHeight, getColor("cinebar"), MC::windowSize.x, barHeight, 0, 0);
    }

    // Handle hand visibility every frame (more reliable than onSetTopScreenName)
    if (getOps<bool>("hidehand")) {
        Option* hideHand = Options::getOption("dev_disableRenderItemInHand");
        if (hideHand != nullptr) {
            hideHand->setvalue(this->active);
        }
    }

    if (!Client::getPlayerState().isInWorld()) return; //

    if (fisrtTime) {
        currentZoomVal = currentFov;
        fisrtTime = false;
        return;
    }

    float animspeed = getOps<float>("anim");
    bool disableanim = getOps<bool>("disableanim");
    bool alwaysanim = getOps<bool>("alwaysanim");

    if (this->active) {
        animationFinished = false;
        if (currentFov > 180) {
            currentZoomVal = disableanim ? currentFov + zoomValue : std::lerp(currentZoomVal, currentFov + zoomValue, animspeed * FlarialGUI::frameFactor);
        } else {
            currentZoomVal = disableanim ? zoomValue : std::lerp(currentZoomVal, zoomValue, animspeed * FlarialGUI::frameFactor);
        }
    } else {
        if ((!animationFinished || alwaysanim) && !disableanim) {
            if (!jdfAnimationFinished) {
                jdfAnimationFinished = true;
            }
            currentZoomVal = std::lerp(currentZoomVal, currentFov, animspeed * FlarialGUI::frameFactor);
            // Check if we're close enough to the target (currentFov) to finish the animation
            if (std::abs(currentFov - currentZoomVal) < unzoomAnimDisableTreshold) {
                animationFinished = true;
                currentZoomVal = currentFov; // Snap to final value to avoid micro-jitters
            }
        } else {
            currentZoomVal = currentFov;
        }
    }
}

void Zoom::onGetFOV(FOVEvent &event) {
    if (!this->isEnabled()) return;
    auto fov = event.getFOV();

    // FOV 70 is the hand FOV - don't modify it during unzoom animation
    // to prevent hand FOV desync
    bool isHandFOV = (fov == 70);

    // Only skip updating currentFov for default values, but still apply zoom
    // This prevents feedback loops while maintaining zoom effect
    if (fov != 70 && fov != 60) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (player) {
            currentFov = fov;
            if (ModuleManager::getModule("Java Dynamic FOV").get()->isEnabled()) {
                if (player->getActorFlag(ActorFlags::FLAG_SPRINTING)) {
                    currentFov = ModuleManager::getModule("Java Dynamic FOV").get()->getOps<float>("fov_target");
                }
            }
        }
    }

    // Apply zoom FOV when active, or during unzoom animation (but not for hand FOV)
    if (this->active) {
        event.setFOV(currentZoomVal);
    } else if (!animationFinished && !isHandFOV) {
        // Only animate main FOV back, let hand FOV return to normal immediately
        event.setFOV(currentZoomVal);
    }
}

void Zoom::onMouse(MouseEvent &event) {
    if (!this->isEnabled()) return;
    if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Press && getOps<bool>("toggleZoom")) {
        keybindActions[0]({});
        if (!getOps<bool>("SaveModifier")) zoomValue = 30.0f;
    } else if (!getOps<bool>("toggleZoom")) {
        if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Press) {
            if (!getOps<bool>("SaveModifier") && !this->active) zoomValue = 30.0f;
            this->active = true;
        }
        else if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Release) this->active = false;
    }

    if (SDK::getCurrentScreen() == "hud_screen" ||
        SDK::getCurrentScreen() == "f1_screen" ||
        SDK::getCurrentScreen() == "zoom_screen" ||
        SDK::getCurrentScreen() == "f3_screen")
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
                    event.getAction() == MouseAction::ScrollDown ||
                    event.getButton() == MouseButton::Scroll) {
                    event.cancel(); // Prevent hotbar scrolling while zooming
                }
            }
        }
}

void Zoom::onKey(KeyEvent &event) {
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) &&
        this->isKeyPartOfKeybind(event.key) &&
        (getOps<bool>("toggleZoom") ? event.getAction() == ActionType::Pressed : true)
        && (SDK::getCurrentScreen() == "hud_screen" || SDK::getCurrentScreen() == "f1_screen" || SDK::getCurrentScreen() == "f3_screen" || SDK::getCurrentScreen() == "zoom_screen")) {
        keybindActions[0]({});
        if (!getOps<bool>("SaveModifier")) zoomValue = 30.0f;
    } else if (getOps<bool>("toggleZoom") ? (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) : !this->isKeybind(event.keys) && (getOps<bool>("toggleZoom") ? event.getAction() == ActionType::Pressed : true)) this->active = false;
}

void Zoom::onSetTopScreenName(SetTopScreenNameEvent &event) {
    if (!this->isEnabled()) return;

    if (this->active) {
        if (getOps<bool>("hidemodules")) {
            event.setCustomLayer("zoom_screen");
        }
        // Hand visibility is now handled in onRender for more reliable updates
    }
    // F1 handling is done globally by F1Listener — no need to duplicate here.
}

void Zoom::onTurnDeltaEvent(TurnDeltaEvent &event) {
    // Only used for cinematic camera smoothing now
    // Sensitivity is handled by onSensitivity via getSensHook
    if (!this->isEnabled() || !this->active) {
        smoothDelta = Vec2<float>{0.f, 0.f};
        return;
    }
    if (getOps<bool>("cinematicMode") && getOps<bool>("smoothing"))
    {
        auto now = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        float alpha = 1.0f - std::exp(-deltaTime * (10.01f - getOps<float>("smoothness")));

        smoothDelta += (event.delta - smoothDelta) * alpha;

        event.delta = smoothDelta;
    }
}

void Zoom::onSensitivity(SensitivityEvent& event)
{
    if (!this->isEnabled() || !this->active) return;
    if (getOps<bool>("cinematicMode")) return; // Cinematic mode uses smoothing instead
    if (!getOps<bool>("lowsens")) return;

    // Calculate zoom ratio and reduce sensitivity proportionally
    // Formula: sensitivity * (zoomValue / currentFov) * strength
    // When zoomed to 30 FOV from 90 FOV, sensitivity becomes ~33% (feels natural)
    float zoomRatio = zoomValue / currentFov;
    float strength = getOps<float>("lowsensStrength");

    // Interpolate between full sensitivity (1.0) and zoom-proportional sensitivity
    float multiplier = 1.0f - (1.0f - zoomRatio) * strength;
    multiplier = std::max(0.01f, std::min(1.0f, multiplier));

    event.setSensitivity(event.getSensitivity() * multiplier);
}
