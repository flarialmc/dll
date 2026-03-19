#pragma once

#include "../HUDModule.hpp"

class BlockBreakIndicator : public HUDModule {
private:
    float lastProgress = 0.0f;
    float currentProgress = 0.0f;
    float currentHeight = 0.0f;

public:
    BlockBreakIndicator() : HUDModule(16, "Break Progress", "Visual indicator to show the progress\nof breaking a block.",
                                      IDR_BLOCK_BREAK_INDICATOR_PNG, "", {"mining", "mine"}) {

    };

    void normalRender(int index, std::string &value) override;

    void settingsRender(float settingsOffset) override;

protected:
    std::string getDisplayValue() override;

    void customConfig() override;
};
