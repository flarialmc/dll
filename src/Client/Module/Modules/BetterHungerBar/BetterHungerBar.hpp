#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"
#include "Utils/Render/PositionUtils.hpp"
#include "../../../../SDK/Client/Block/BlockLegacy.hpp"
#include "../../../../SDK/Client/Block/BlockSource.hpp"
#include "../../../../SDK/Client/Block/Block.hpp"

#include <string>

class BetterHungerBar : public Module {
private:

    int gameMode = 0;
    int currentHunger = 0;
    int predictedHunger = 0;
    
    double currentSaturation = 0;
    double predictedSaturation = 0;
    double getCurrentTime();
    
    bool hasHungerEffect = false;
    
    std::string itemName = "";
    
public:

    BetterHungerBar() : Module("Better Hunger Bar", "Adds a hunger and saturation overlay similar to AppleSkin.",
        IDR_BETTERHUNGERBAR_PNG, "") {
        
    };
    
    void onEnable() override;
    
    void onDisable() override;
    
    void defaultConfig() override;
    
    void settingsRender(float settingsOffset) override;
    
    void onTick(TickEvent& event);

    void onSetupAndRender(const SetupAndRenderEvent& event);
    
    int getPredictedHunger();
    
    double getPredictedSaturation();

};