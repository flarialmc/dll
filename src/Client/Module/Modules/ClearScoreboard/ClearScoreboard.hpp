#pragma once

#include "../Module.hpp"

class ClearScoreboard : public Module {
public:
    ClearScoreboard() : Module("Clear Scoreboard", "Clears the scoreboard from the screen.",
        IDR_CLEARBG_PNG, "") {
    }

    void onEnable() override;
    void onDisable() override;
    void defaultConfig() override;
    void settingsRender(float settingsOffset) override;
    void onSetupAndRender(SetupAndRenderEvent& event);

private:
    static inline std::string layerName = "sidebar";
    std::shared_ptr<UIControl> targetControl = nullptr;
    Vec2<float> originalSize = Vec2{0.f, 0.f};
    bool savedOriginalSize = false;
    void clearScoreboard();
    void restoreScoreboard();
};