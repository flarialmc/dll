// Updated 1:54 UTC+8 by Leqixn

#pragma once
#include "Modules/Module.hpp"
#include <optional>

class DynamicFPS : public Module {
public:
    DynamicFPS() : Module("DynamicFPS", "Misc", "Reduces FPS while AFK or on other Tab") {
        this->name = "DynamicFPS"; 
        
        addSetting(&enabled);
        addSetting(&unfocusedFPS);
        addSetting(&afkFPS);
        addSetting(&afkTimeout);
    }

    Setting enabled = Setting("Enabled", true);
    Setting unfocusedFPS = Setting("Unfocused FPS", 20.0f, 1.0f, 60.0f);
    Setting afkFPS = Setting("AFK FPS", 30.0f, 1.0f, 60.0f);
    Setting afkTimeout = Setting("AFK Timeout", 60.0f, 5.0f, 300.0f);

    void onTick() override;
    void onDisable() override;

private:
    bool isAFK() const;
    
    std::optional<int> originalLimit;
    bool isThrottled = false;
};
