// TODO: - only proceed if not in f1 screen
// TODO: - auto detect the icon locations in texture packs instead of relying on the user

#include "BetterHungerBar.hpp"
#include "Events/EventManager.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Client.hpp"
#include "Utils/Render/PositionUtils.hpp"

#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <chrono>

void BetterHungerBar::onEnable() {
    Listen(this, TickEvent, &BetterHungerBar::onTick)
    Listen(this, SetupAndRenderEvent, &BetterHungerBar::onSetupAndRender)
    Module::onEnable();
}

void BetterHungerBar::onDisable() {
    Deafen(this, TickEvent, &BetterHungerBar::onTick)
    Deafen(this, SetupAndRenderEvent, &BetterHungerBar::onSetupAndRender)
    Module::onDisable();
}

void BetterHungerBar::defaultConfig() {
	settings.changeType<float, int>("xOffset");
	settings.changeType<float, int>("yOffset");
    Module::defaultConfig("core");
    setDef("saturationColor", (std::string)"FFBB00", 1.f, false);
    setDef("fadeSpeed", 7.f);
    setDef("maxFadeOpacity", 185.f);
    setDef("showOnFullHunger", true);
    setDef("newTextureSystem", false);
    setDef("xOffset", 82);
    setDef("yOffset", 41);
    setDef("scale", 1.f);
}

void BetterHungerBar::settingsRender(float settingsOffset) {

    initSettingsPage();

    addHeader("Main");
    addColorPicker("Saturation Color", "", "saturationColor");
    addSlider("Fade Speed", "", "fadeSpeed", 10.0f);
    addSlider("Max Fade Opacity", "", "maxFadeOpacity", 255.0f);
    addToggle("Show on full hunger", "Displays predicted saturation despite not being able\nto eat the held food.", "showOnFullHunger");

    extraPadding();
    
    addHeader("Manual Fix");
    // quirky solution but honestly i give up trying to make this auto-detect
    addToggle("New Texture System", "Enable/Disable this if you experience overlapping/issues\nwith texture packs!!", "newTextureSystem");

    // not really necessary but eh you never know
    addSliderInt("xOffset", "", "xOffset", 500);
    addSliderInt("yOffset", "", "yOffset", 500);
    addSlider("Scale", "", "scale", 5.0f);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

struct Food { int hunger; double saturation; };

// credits to Zonrox for the values <3
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

    // Only proceed if in survival, adventure or default
    auto gameModeComponent = player->getGameModeType();
    if (!gameModeComponent) return;
    
    gameMode = static_cast<int>(gameModeComponent->gameType);
    if (gameMode != 0 && gameMode != 2 && gameMode != 5) {
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
                    if (!heldItemIsFood || itemName == "nothing") {
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

void BetterHungerBar::onSetupAndRender(const SetupAndRenderEvent &event) {
    if (!this->isEnabled() ||
        !SDK::hasInstanced ||
        SDK::clientInstance == nullptr ||
        SDK::getCurrentScreen() != "hud_screen" ||
        (gameMode != 0 && gameMode != 2 && gameMode != 5)
    ) return;

    auto muirc = event.getMuirc();
    
    loadTextures(muirc);
    
    // Clean "icons.png" by only keeping the outline for saturation instead of the full icon
    auto fallbackLocation = ResourceLocation("textures/gui/icons", false);
    ResourceLocation cleanSatOutline = getOutlineG(fallbackLocation);
    auto fallbackTexture = muirc->createTexture(cleanSatOutline, false);
    
    // Used for the fade/pulse animation in 1.2, 2.2 and 3.2
    double time_seconds = getCurrentTime();
    float maxFadeOpacity = getOps<float>("maxFadeOpacity");
    float fadeSpeed = getOps<float>("fadeSpeed");
    int alpha = static_cast<int>(std::floor((maxFadeOpacity / 2.0f) * (1 + std::sin(time_seconds * fadeSpeed))));
    
    D2D1_COLOR_F baseColor = getColor("saturationColor");
    mce::Color satColorSolid(baseColor.r, baseColor.g, baseColor.b, 1.0f);
    mce::Color satColorTransparent(baseColor.r, baseColor.g, baseColor.b);
    mce::Color defaultColor;

    float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
    float manualscale = getOps<float>("scale");

    float iconSize = 9.0f * manualscale;
    float iconSpacing = 8.0f * manualscale;
    
    auto currentPos = Vec2<float>(
        (MC::windowSize.x / 2 + (getOps<int>("xOffset") * guiscale)), 
        (MC::windowSize.y - (getOps<int>("yOffset") * guiscale))
    );

    Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
    Vec2<float> uvSize = Vec2<float>((9.f/256.f), (9.f/256.f));
    Vec2<float> uvSizeSaturationHalf = Vec2<float>((5.f/256.f), (9.f/256.f));
    Vec2<float> uvHungerFull = Vec2<float>((52.f / 256.f), (27.f / 256.f));
    Vec2<float> uvHungerHalf = Vec2<float>((61.f / 256.f), (27.f / 256.f));
    Vec2<float> uvRottenFull = Vec2<float>((88.f / 256.f), (27.f / 256.f));
    Vec2<float> uvRottenHalf = Vec2<float>((97.f / 256.f), (27.f / 256.f));
    Vec2<float> uvSaturationFull = Vec2<float>((25.f / 256.f), (27.f / 256.f));
    Vec2<float> uvSaturationHalf = Vec2<float>((29.f / 256.f), (27.f / 256.f));
    Vec2<float> uvOutlineNormal = Vec2<float>((16.f / 256.f), (27.f / 256.f));
    Vec2<float> uvOutlineHunger = Vec2<float>((133.f / 256.f), (27.f / 256.f));

    useNewTextures = getOps<bool>("newTextureSystem");
    
    auto getTexture = [&](const std::string& type) -> TexturePtr {
        if (useNewTextures) {
            if (type == "outline_normal") return hungerBackgroundTexture;
            if (type == "outline_hunger") return hungerEffectBackgroundTexture;
            if (type == "hunger_full") return hungerFullTexture;
            if (type == "hunger_half") return hungerHalfTexture;
            if (type == "rotten_full") return hungerEffectFullTexture;
            if (type == "rotten_half") return hungerEffectHalfTexture;
            if (type == "saturation") return hungerBlinkTexture;
        }
        return fallbackTexture;
    };

    auto getUV = [&](const std::string& type) -> Vec2<float> {
        if (useNewTextures) {
            if (type == "saturation_half") return Vec2<float>((4.f/9.f), 0);
            return Vec2<float>(0, 0);
        }
        if (type == "outline_normal") return uvOutlineNormal;
        if (type == "outline_hunger") return uvOutlineHunger;
        if (type == "hunger_full") return uvHungerFull;
        if (type == "hunger_half") return uvHungerHalf;
        if (type == "rotten_full") return uvRottenFull;
        if (type == "rotten_half") return uvRottenHalf;
        if (type == "saturation_full") return uvSaturationFull;
        if (type == "saturation_half") return uvSaturationHalf;
    };

    auto getUVSize = [&](const std::string& type) -> Vec2<float> {
        if (useNewTextures) {
            if (type == "saturation_half") return Vec2<float>((5.f/9.f), 1.0f);
            return Vec2<float>(1.0f, 1.0f);
        }
        if (type == "saturation_half") return uvSizeSaturationHalf;
        return uvSize;
    };

    static auto flushLayer = HashedString("ui_flush");
    
    // Render all 10 icons one by one from right to left
    for (int i = 0; i < 10; i++) {
        
        float currentX = scaledPos.x - (i * iconSpacing);
        float currentY = scaledPos.y;
        
        auto position = Vec2<float>(currentX, currentY);
        auto size = Vec2<float>(iconSize, iconSize);
        
        // 1.1) Render outline icons
        std::string outlineType = hasHungerEffect ? "outline_hunger" : "outline_normal";
		TexturePtr outlineTexture = getTexture(outlineType);
        Vec2<float> outlineUVSize = getUVSize(outlineType);
		Vec2<float> outlineUV = getUV(outlineType);
        
        muirc->drawImage(outlineTexture, position, size, outlineUV, outlineUVSize);
        muirc->flushImages(defaultColor, 1.0f, flushLayer);
        

        // 1.2) Render predicted outline icons
        if (itemName == "milk_bucket") {
			TexturePtr normalOutlineTexture = getTexture("outline_normal");
			Vec2<float> normalOutlineUVSize = getUVSize("outline_normal");
            Vec2<float> normalOutlineUV = getUV("outline_normal");
            
            muirc->drawImage(normalOutlineTexture, position, size, normalOutlineUV, normalOutlineUVSize);
            muirc->flushImages(defaultColor, (alpha / 255.0f), flushLayer);
        }
        

        // 2.1) Render current hunger icons
        float hungerValue = currentHunger / 2.0f;
        if (hungerValue > i) {
            bool showFullIcon = (hungerValue - i) >= 1.0f;
            
            std::string hungerType;
            if (hasHungerEffect) {
                hungerType = showFullIcon ? "rotten_full" : "rotten_half";
            } else {
                hungerType = showFullIcon ? "hunger_full" : "hunger_half";
            }
            
			TexturePtr hungerTexture = getTexture(hungerType);
			Vec2<float> hungerUVSize = getUVSize(hungerType);
            Vec2<float> hungerUV = getUV(hungerType);
            
            muirc->drawImage(hungerTexture, position, size, hungerUV, hungerUVSize);
            muirc->flushImages(defaultColor, 1.0f, flushLayer);
        }
        
		
        // 2.2) Render predicted hunger icons
        float predictedHungerValue = predictedHunger / 2.0f;
        if (predictedHungerValue > i) {
            bool showFullIcon = (predictedHungerValue - i) >= 1.0f;
            
            std::string hungerType;
            if (itemName == "milk_bucket") {
                hungerType = showFullIcon ? "hunger_full" : "hunger_half";
            } else {
                if (hasHungerEffect) {
                    hungerType = showFullIcon ? "rotten_full" : "rotten_half";
                } else {
                    hungerType = showFullIcon ? "hunger_full" : "hunger_half";
                }
            }
            
			TexturePtr hungerTexture = getTexture(hungerType);
			Vec2<float> hungerUVSize = getUVSize(hungerType);
            Vec2<float> hungerUV = getUV(hungerType);
            
            muirc->drawImage(hungerTexture, position, size, hungerUV, hungerUVSize);
            muirc->flushImages(defaultColor, (alpha / 255.0f), flushLayer);
        }


		// 3.1) Render current saturation outline icons
        if (std::floor(currentSaturation) / 2 > i) {
            bool fullIcon = i != (std::floor(currentSaturation) - 1) / 2;
            std::string satType = fullIcon ? "saturation_full" : "saturation_half";
            
            // positioning and sizing for half icons
            Vec2<float> adjustedPosition = position;
            Vec2<float> adjustedSize = size;
            
            if (!fullIcon) {
                adjustedPosition.x += 4.0f * (iconSize / 9.0f);
                adjustedSize.x = (5.0f / 9.0f) * iconSize;
            }
            
			TexturePtr satTexture = getTexture("saturation");
			Vec2<float> satUVSize = getUVSize(satType);
            Vec2<float> satUV = getUV(satType);
            
            muirc->drawImage(satTexture, adjustedPosition, adjustedSize, satUV, satUVSize);
            muirc->flushImages(satColorSolid, 1.0f, flushLayer);
        }
        

        // 3.2) Render predicted saturation outline icons
        bool shouldShowPredictedSaturation;
        if (currentHunger == 20) {
            if (getOps<bool>("showOnFullHunger")) {
                shouldShowPredictedSaturation = true;
            } else {
                // Only show for special consumable items when showOnFullHunger is false
                shouldShowPredictedSaturation = (itemName == "golden_apple" || 
                                               itemName == "enchanted_golden_apple" || 
                                               itemName == "honey_bottle");
            }
        } else {
            shouldShowPredictedSaturation = true;
        }

        if (shouldShowPredictedSaturation && predictedSaturation / 2 > i) {
            bool fullIcon = i != (predictedSaturation - 1) / 2;
            std::string satType = fullIcon ? "saturation_full" : "saturation_half";
            
            // positioning and sizing for half icons
            Vec2<float> adjustedPosition = position;
            Vec2<float> adjustedSize = size;
            
            if (!fullIcon) {
                adjustedPosition.x += 4.0f * (iconSize / 9.0f);
                adjustedSize.x = (5.0f / 9.0f) * iconSize;
            }
            
			TexturePtr satTexture = getTexture("saturation");
			Vec2<float> satUVSize = getUVSize(satType);
            Vec2<float> satUV = getUV(satType);
            
            muirc->drawImage(satTexture, adjustedPosition, adjustedSize, satUV, satUVSize);
            muirc->flushImages(satColorTransparent, (alpha / 255.0f), flushLayer);
        }
    }
}

void BetterHungerBar::loadTextures(MinecraftUIRenderContext* muirc) {
	
    auto hungerFullLoc = ResourceLocation("textures/ui/hunger_full", false);
    auto hungerHalfLoc = ResourceLocation("textures/ui/hunger_half", false);
    auto hungerEffectFullLoc = ResourceLocation("textures/ui/hunger_effect_full", false);
    auto hungerEffectHalfLoc = ResourceLocation("textures/ui/hunger_effect_half", false);
    auto hungerBackgroundLoc = ResourceLocation("textures/ui/hunger_background", false);
    auto hungerEffectBackgroundLoc = ResourceLocation("textures/ui/hunger_effect_background", false);

    // Clean "hunger_blink.png" by only keeping the white non-transparent pixels (= outline)
	auto hungerBlinkLoc = ResourceLocation("textures/ui/hunger_blink", false);
    ResourceLocation cleanSatOutline2 = getOutlineU(hungerBlinkLoc);
    hungerBlinkTexture = muirc->createTexture(cleanSatOutline2, false);

    hungerFullTexture = muirc->createTexture(hungerFullLoc, false);
    hungerHalfTexture = muirc->createTexture(hungerHalfLoc, false);
    hungerEffectFullTexture = muirc->createTexture(hungerEffectFullLoc, false);
    hungerEffectHalfTexture = muirc->createTexture(hungerEffectHalfLoc, false);
    hungerBackgroundTexture = muirc->createTexture(hungerBackgroundLoc, false);
    hungerEffectBackgroundTexture = muirc->createTexture(hungerEffectBackgroundLoc, false);
}