// TODO - only proceed if not in f1 screen
// TODO - use dynamic icons based on texture pack
// TODO - place icons correctly and dynamically change pos based on screen size

#include "BetterHungerBar.hpp"

#include "Events/EventManager.hpp"
#include "Events/Game/TickEvent.hpp"
#include <Client.hpp>

#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <chrono>

void BetterHungerBar::onEnable() {
    Listen(this, TickEvent, &BetterHungerBar::onTick)
    Listen(this, RenderEvent, &BetterHungerBar::onRender)
    Module::onEnable();
}

void BetterHungerBar::onDisable() {
    Deafen(this, TickEvent, &BetterHungerBar::onTick)
    Deafen(this, RenderEvent, &BetterHungerBar::onRender)
    Module::onDisable();
}

void BetterHungerBar::defaultConfig() {
    Module::defaultConfig("core");
    setDef("saturationColor", (std::string)"FFBB00", 1.f, false);
    setDef("fadeSpeed", 7.f);
    setDef("prioritizeCake", false);
    setDef("manualFix", false);
    setDef("xOffset", 246.f);
    setDef("yOffset", 123.f);
    setDef("scale", 1.f);
}

void BetterHungerBar::settingsRender(float settingsOffset) {

    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Main");
    addColorPicker("Saturation Color", "", "saturationColor");
    addSlider("Fade Speed", "", "fadeSpeed", 10.0f);
    addToggle("Prioritize Cake", "When looking at a cake, the cake values override the hold-food values.", "prioritizeCake");
    
    extraPadding();
    
    addHeader("Misc");
    addToggle("Manual Fix", "Enable this and adjust the values below manually if you experience issues\nwith the positioning/size of the icons!", "manualFix");
    if (getOps<bool>("manualFix"))
    {
        addSlider("xOffset [Use Integers]", "", "xOffset", 500.0f);
        addSlider("yOffset [Use Integers]", "", "yOffset", 500.0f);
        addSlider("Scale [Use Integers]", "", "scale", 2.0f);
    }

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

struct Food { int hunger; double saturation; };

std::map<std::string, Food> foods = {
    {"apple",                  {4, 2.0}},
    {"baked_potato",           {5, 6.0}},
    {"beetroot",               {1, 1.2}},
    {"beetroot_soup",          {6, 7.0}},
    {"bread",                  {5, 6.0}},
    {"cake",                   {2, 0.4}},
    {"carrot",                 {3, 4.0}},
    {"chorus_fruit",           {4, 2.0}},
    {"cooked_beef",            {8, 12.8}},
    {"cooked_chicken",         {6, 7.0}},
    {"cooked_cod",             {5, 6.0}},
    {"cooked_mutton",          {6, 10.0}},
    {"cooked_porkchop",        {8, 12.8}},
    {"cooked_rabbit",          {5, 6.0}},
    {"cooked_salmon",          {6, 9.6}},
    {"cookie",                 {2, 0.4}},
    {"dried_kelp",             {1, 0.2}},
    {"enchanted_golden_apple", {4, 9.6}},
    {"glow_berries",           {2, 1.2}},
    {"golden_apple",           {4, 9.6}},
    {"golden_carrot",          {6, 14.4}},
    {"honey_bottle",           {6, 1.2}},
    {"melon_slice",            {2, 1.2}},
    {"mushroom_stew",          {6, 7.2}},
    {"poisonous_potato",       {2, 1.2}},
    {"potato",                 {1, 0.4}},
    {"pufferfish",             {1, 0.2}},
    {"pumpkin_pie",            {8, 4.8}},
    {"rabbit_stew",            {10, 12.0}},
    {"beef",                   {3, 1.8}},
    {"chicken",                {2, 1.2}},
    {"cod",                    {2, 0.4}},
    {"mutton",                 {2, 1.2}},
    {"porkchop",               {3, 1.8}},
    {"rabbit",                 {3, 1.8}},
    {"salmon",                 {2, 0.4}},
    {"rotten_flesh",           {4, 0.8}},
    {"spider_eye",             {2, 3.2}},
    {"suspicious_stew",        {6, 7.2}},
    {"sweet_berries",          {2, 1.2}},
    {"tropical_fish",          {1, 0.2}},
    {"milk_bucket",            {0, 0}},
    {"nothing",                {0, 0}}
};

void BetterHungerBar::onTick(TickEvent& event) {
    if (!this->isEnabled() ||
        !SDK::hasInstanced ||
        SDK::clientInstance == nullptr ||
        SDK::getCurrentScreen() != "hud_screen"
    ) return;

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player) return;

	// Only proceed if in survival (0) or adventure (2) mode
    auto gameModeComponent = player->getGameModeType();
    if (!gameModeComponent) return;
    
    gameMode = static_cast<int>(gameModeComponent->gameType);
    if (gameMode != 0 && gameMode != 2) {
        return;
    }
	
    currentHunger = player->getHunger();
    currentSaturation = player->getSaturation();

    predictedHunger = 0;
    predictedSaturation = 0;

    // Check if the player has the Hunger effect
    std::vector<UnifiedMobEffectData> effects = player->getMobEffects();
    hasHungerEffect = false;
    
    for (const auto& effect : effects) {
        if (!effect.isValid()) continue;
        
        std::string effectName = effect.getName();
        if (effectName == "Hunger") {
            hasHungerEffect = true;
            break;
        }
    }

    if (!player->getSupplies()) return;
    auto supplies = player->getSupplies();
    auto inventory = supplies->getInventory();
    if (!inventory) return;

    auto selectedSlot = supplies->getSelectedSlot();
    auto itemStack = inventory->getItem(selectedSlot);

    // Get the held item name first
    if (!itemStack || !itemStack->getItem()) {
        itemName = "nothing";
    } else {
        itemName = itemStack->getItem()->name;
    }

    bool heldItemIsFood = (foods.find(itemName) != foods.end());

    // Cake detection
    try {
        if (player->getLevel() && SDK::clientInstance->getBlockSource()) {
            HitResult result = player->getLevel()->getHitResult();
            
            BlockPos pos = {
                result.blockPos.x,
                result.blockPos.y,
                result.blockPos.z
            };
                
            BlockSource* blockSource = SDK::clientInstance->getBlockSource();
            BlockLegacy* block = blockSource->getBlock(pos)->getBlockLegacy();
            
            if (block) {
                std::string blockName = block->getName();
                
                if (blockName == "cake") {
                    if ((!heldItemIsFood && !getOps<bool>("prioritizeCake")) || itemName == "nothing" || getOps<bool>("prioritizeCake")) {
                        itemName = "cake";
                    }
                }
            }
        }
    }
    catch (...) {}

    if (foods.find(itemName) != foods.end()) {
        predictedHunger = getPredictedHunger();
        predictedSaturation = getPredictedSaturation();
    }
}

int BetterHungerBar::getPredictedHunger() {
    auto food = foods[itemName];
    int newHunger = currentHunger + food.hunger;
    
    // Hunger can maximally be 20
    return std::min(newHunger, 20);
}

double BetterHungerBar::getPredictedSaturation() {
    auto food = foods[itemName];
    double newSaturation = currentSaturation + food.saturation;
    double newHunger = currentHunger + food.hunger;
    
    // Saturation can maximally be 20
    return std::floor(std::min(newSaturation, newHunger));
}

double BetterHungerBar::getCurrentTime() {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();

    return std::chrono::duration<double>(duration).count();
}

void BetterHungerBar::onRender(RenderEvent& event) {
    if (
        !this->isEnabled() || 
        SDK::getCurrentScreen() != "hud_screen" ||
		(gameMode != 0 && gameMode != 2)
    ) return;
    
    // Used for the fade/pulse animation in 1.2, 2.2 and 3.2
    double time_seconds = getCurrentTime();
    int alpha = static_cast<int>(std::floor(100 + (255-100) * std::sin(time_seconds * getOps<float>("fadeSpeed"))));
    
    D2D1_COLOR_F baseColor = getColor("saturationColor");
    
    D2D1_COLOR_F satColorSolid = baseColor;
    satColorSolid.a = 1.0f; // ensure full opacity

    D2D1_COLOR_F satColorTransparent = baseColor;
    satColorTransparent.a = alpha / 255.0f; // normalize

    // TODO somehow find out how to make this dynamic based on screen size
    float positionX = MC::windowSize.x / 2 + getOps<float>("xOffset");
    float positionY = MC::windowSize.y - getOps<float>("yOffset");
    
    float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
    float iconSize = 36.0f * (guiscale / 4) * getOps<float>("scale");
    float iconSpacing = 32.0f * (guiscale / 4) * getOps<float>("scale");

    // Render all 10 icons one by one from right to left
    for (int i = 0; i < 10; i++) {
        
        float currentX = positionX - (i * iconSpacing);
        float currentY = positionY;

        // 1.1) Render outline icons
        auto icon1 = hasHungerEffect ? 278 : 277;
        FlarialGUI::image(icon1, D2D1::RectF(currentX, currentY, currentX + iconSize, currentY + iconSize), "PNG");


        // 1.2) Render predicted outline icons
        if (itemName == "milk_bucket") {
            FlarialGUI::image(277, D2D1::RectF(currentX, currentY, currentX + iconSize, currentY + iconSize), "PNG", true, ImColor(255, 255, 255, alpha));
        }
        

        // 2.1) Render current saturation outline icons
        if (std::floor(currentSaturation) / 2 > i) {
            bool fullIcon2 = i != (std::floor(currentSaturation) - 1) / 2;
            auto icon2 = fullIcon2 ? 275 : 276;
            FlarialGUI::image(icon2, D2D1::RectF(currentX, currentY, currentX + iconSize, currentY + iconSize), "PNG", true, FlarialGUI::D2DColorToImColor(satColorSolid));
        }

 
        // 2.2) Render predicted saturation outline icons
        if (predictedSaturation / 2 > i) {
            bool fullIcon3 = i != (predictedSaturation - 1) / 2;
            auto icon3 = fullIcon3 ? 275 : 276;
            FlarialGUI::image(icon3, D2D1::RectF(currentX, currentY, currentX + iconSize, currentY + iconSize), "PNG", true, FlarialGUI::D2DColorToImColor(satColorTransparent));
        }


        // 3.1) Render current hunger icons
        float hungerValue = currentHunger / 2.0f;
        if (hungerValue > i) {

            bool showFullIcon = (hungerValue - i) >= 1.0f;
    
            auto iconFood = showFullIcon ? 271 : 272;
            auto iconRotten = showFullIcon ? 273 : 274;
            auto icon4 = hasHungerEffect ? iconRotten : iconFood;
            FlarialGUI::image(icon4, D2D1::RectF(currentX, currentY, currentX + iconSize, currentY + iconSize), "PNG");
        }

 
        // 3.2) Render predicted hunger icons
        float predictedHungerValue = predictedHunger / 2.0f;
        if (predictedHungerValue > i) {
            
            bool showFullIcon = (predictedHungerValue - i) >= 1.0f;
            
            if (itemName == "milk_bucket") {
                auto milkSpecialHandling = showFullIcon ? 271 : 272;
                FlarialGUI::image(milkSpecialHandling, D2D1::RectF(currentX, currentY, currentX + iconSize, currentY + iconSize), "PNG", true, ImColor(255, 255, 255, alpha));

            } else {
                auto iconFood = showFullIcon ? 271 : 272;
                auto iconRotten = showFullIcon ? 273 : 274;
                auto icon5 = hasHungerEffect ? iconRotten : iconFood;
                FlarialGUI::image(icon5, D2D1::RectF(currentX, currentY, currentX + iconSize, currentY + iconSize), "PNG", true, ImColor(255, 255, 255, alpha));
            }
        }
    }
}