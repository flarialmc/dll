#pragma once

#include "../HUDModule.hpp"
#include "Events/Game/TickEvent.hpp"

class ArrowCounter : public HUDModule {
public:
    ArrowCounter() : HUDModule(13, "Arrow Counter", "Counts how many arrows you have\nin your inventory.",
        IDR_ARROW_PNG, "") {}

protected:
    std::string getDisplayValue() override;

    void customConfig() override;

    void customSettings() override;

    void customInit() override;

    void customCleanup() override;

private:
    int arrows = 0;
    bool shouldRender = true;
    int lastArrowCount = 0;
    int tickCounter = 0;

    void onTick(TickEvent& event);
};
