#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"

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
    void clearScoreboard();
    void restoreScoreboard();
};