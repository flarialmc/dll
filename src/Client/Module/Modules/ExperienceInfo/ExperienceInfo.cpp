#include "ExperienceInfo.hpp"

void ExperienceInfo::onEnable() {
    HUDModule::onEnable();
    Listen(this, TickEvent, &ExperienceInfo::onTick)
}

void ExperienceInfo::onDisable() {
    Deafen(this, TickEvent, &ExperienceInfo::onTick)
    HUDModule::onDisable();
}

std::string ExperienceInfo::getDisplayValue() {
    return displayValue;
}

void ExperienceInfo::customConfig() {
    setDef("text", (std::string)"XP: {value}");
    setDef("displayMode", (std::string)"Level & Percent");
    setDef("showRemaining", false);
    setDef("showDecimal", false);
}

void ExperienceInfo::customSettings() {
    addDropdown("Display Mode", "Choose what XP info to show.",
        {"Level & Percent", "Decimal Level", "XP Counts", "Comprehensive"},
        "displayMode", true);
    std::string mode = getOps<std::string>("displayMode");
    addConditionalToggle(mode == "XP Counts" || mode == "Comprehensive",
        "Show Remaining", "Show remaining XP to next level.", "showRemaining");
    addConditionalToggle(mode == "Comprehensive",
        "Decimal Level", "Show level as a decimal (e.g. 25.95) instead of integer.", "showDecimal");
}

void ExperienceInfo::onTick(TickEvent& event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return;

    auto* player = SDK::clientInstance->getLocalPlayer();

    int level = player->getPlayerLevel();
    float progress = player->getExperienceProgress();

    // Clamp progress to valid range
    if (progress < 0.f) progress = 0.f;
    if (progress > 1.f) progress = 1.f;

    float percent = progress * 100.f;
    int xpNeeded = computeXpForLevel(level);
    int currentXp = static_cast<int>(progress * xpNeeded);
    int remaining = xpNeeded - currentXp;

    std::string mode = getOps<std::string>("displayMode");
    bool showRemaining = getOps<bool>("showRemaining");

    if (mode == "Decimal Level") {
        displayValue = std::format("{:.2f}", level + progress);
    } else if (mode == "XP Counts") {
        if (showRemaining)
            displayValue = std::format("{}/{} ({} left)", currentXp, xpNeeded, remaining);
        else
            displayValue = std::format("{}/{}", currentXp, xpNeeded);
    } else if (mode == "Comprehensive") {
        bool showDecimal = getOps<bool>("showDecimal");
        std::string lvl = showDecimal ? std::format("{:.2f}", level + progress) : std::to_string(level);
        if (showRemaining)
            displayValue = std::format("Lv. {} | {}/{} ({:.1f}%) [{} left]", lvl, currentXp, xpNeeded, percent, remaining);
        else
            displayValue = std::format("Lv. {} | {}/{} ({:.1f}%)", lvl, currentXp, xpNeeded, percent);
    } else { // Level & Percent (default)
        displayValue = std::format("{} ({:.1f}%)", level, percent);
    }
}
