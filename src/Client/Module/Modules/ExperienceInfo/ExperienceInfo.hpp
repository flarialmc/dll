#pragma once

#include "../HUDModule.hpp"
#include "Events/Game/TickEvent.hpp"

class ExperienceInfo : public HUDModule {

private:
    std::string displayValue;

    static int computeXpForLevel(int level) {
        if (level < 15) return 2 * level + 7;
        if (level < 30) return 5 * level - 38;
        return 9 * level - 158;
    }

public:
    ExperienceInfo() : HUDModule(41, "Experience Info",
        "Displays detailed XP information including\nlevel, progress, and XP counts.",
        IDR_LIKE_PNG, "", {"xp", "experience", "xpinfo"}) {
    }

    void onEnable() override;
    void onDisable() override;
    void onTick(TickEvent& event);

protected:
    std::string getDisplayValue() override;
    void customConfig() override;
    void customSettings() override;
};
