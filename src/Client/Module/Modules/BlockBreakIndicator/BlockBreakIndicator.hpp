#pragma once

#include "../Module.hpp"

#include "../../../../Assets/Assets.hpp"
#include "Events/Events.hpp"

class BlockBreakIndicator : public Module {
private:
    float lastProgress = 0.0f;
    float currentProgress = 0.0f;

public:
    BlockBreakIndicator() : Module("Break Progress", "Visual indicator to show the progress\nof breaking a block.",
                                   IDR_BLOCK_BREAK_INDICATOR_PNG, "", false, {"mining", "mine"}) {
        
    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    float currentHeight = 0.0f;

    void normalRender(int index, std::string &value) override;

    void onRender(RenderEvent &event);
};
