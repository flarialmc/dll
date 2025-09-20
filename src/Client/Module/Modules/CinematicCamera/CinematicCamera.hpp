#pragma once

#include "../Module.hpp"
#include "Events/Game/TurnDeltaEvent.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "../../../../Assets/Assets.hpp"
#include "../../../../Utils/Utils.hpp"

class CinematicCamera : public Module {
private:
    Vec2<float> smoothDelta{0, 0};
    std::chrono::time_point<std::chrono::steady_clock> lastTime = std::chrono::steady_clock::now();

public:
    CinematicCamera() : Module("Cinematic Camera", "Smooth Cinema-like Camera Movement",
        IDR_CINEMATICCAMERA_PNG, "", false, {"smooth"}) {
    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onTurnDelta(TurnDeltaEvent &event);

    void onRender(RenderEvent &event);
};
