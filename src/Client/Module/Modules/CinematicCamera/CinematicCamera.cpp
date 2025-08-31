#include "CinematicCamera.hpp"
#include "Events/EventManager.hpp"
#include "SDK/Client/Core/Options.hpp"

void CinematicCamera::onEnable() {
    Listen(this, TurnDeltaEvent, &CinematicCamera::onTurnDelta)
    Listen(this, RenderEvent, &CinematicCamera::onRender)
    Module::onEnable();
}

void CinematicCamera::onDisable() {
    Deafen(this, RenderEvent, &CinematicCamera::onRender)
    Module::onDisable();
}

void CinematicCamera::defaultConfig() {
    setDef("smoothing", true);
    setDef("smoothness", 8.f);
    setDef("cinebars", false);
    setDef("cinebar", "000000", 1.f, false);
    setDef("cinebarHeight", 0.2f);
    Module::defaultConfig("core");
}

void CinematicCamera::settingsRender(float settingsOffset) {
    /* Border Start */

    initSettingsPage();

    addHeader("Cinematic Camera");
    addToggle("Enable Smoothing", "", "smoothing");
    addConditionalSlider(getOps<bool>("smoothing"), "Smoothing", "", "smoothness", 10.f);
    addToggle("Cinematic Bars", "", "cinebars");
    addConditionalSlider(getOps<bool>("cinebars"), "Cinematic Bar Height", "", "cinebarHeight", 0.8f);
    addConditionalColorPicker(getOps<bool>("cinebars"), "Cinematic Bar Color", "", "cinebar");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void CinematicCamera::onTurnDelta(TurnDeltaEvent &event) {
    if (!isEnabled() || !getOps<bool>("smoothing")) {
        smoothDelta = Vec2<float>{0.f, 0.f};
        return;
    };

    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;

    float alpha = 1.0f - std::exp(-deltaTime * (10.01f - getOps<float>("smoothness")));

    smoothDelta += (event.delta - smoothDelta) * alpha;

    event.delta = smoothDelta;
}

void CinematicCamera::onRender(RenderEvent &event) {
    if (!isEnabled() || !getOps<bool>("cinebars") || (SDK::getCurrentScreen() != "hud_screen" && SDK::getCurrentScreen() != "zoom_screen" && SDK::getCurrentScreen() != "f3_screen")) return;

    float barHeight = MC::windowSize.y * getOps<float>("cinebarHeight") / 2.f;

    FlarialGUI::RoundedRect(0, 0, getColor("cinebar"), MC::windowSize.x, barHeight, 0, 0);
    FlarialGUI::RoundedRect(0, MC::windowSize.y - barHeight, getColor("cinebar"), MC::windowSize.x, barHeight, 0, 0);
}
