#include "PotionHUD.hpp"

#include "Events/EventManager.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Utils/Render/PositionUtils.hpp"

PotionHUD::PotionHUD(): Module("PotionHUD", "Displays your potion effects",
                               IDR_POTION_PNG, "")
{
    
}

void PotionHUD::onEnable()
{
    Listen(this, RenderEvent, &PotionHUD::onRender)
    Listen(this, RenderPotionHUDEvent, &PotionHUD::onRenderPotionHUD)
    Listen(this, SetupAndRenderEvent, &PotionHUD::onSetupAndRender)
    if (FlarialGUI::inMenu) {
        FlarialGUI::Notify("To change the position of PotionHUD, Please click " +
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "editmenubind")->value + " in the settings tab.");
    }
    Module::onEnable();
}

void PotionHUD::onDisable()
{

    Module::onDisable();
}

void PotionHUD::defaultConfig()
{
    settings.renameSetting("colorMain", "colorMain_opacity", "colorMain_rgb", "main");
    settings.renameSetting("colorLow", "colorLow_opacity", "colorLow_rgb", "low");
    Module::defaultConfig("core");
    Module::defaultConfig("pos");
    setDef("uiscale", 1.0f);
    setDef("textSize", 0.05f);
    setDef("main", (std::string)"FFFFFF", 1.f, false);
    setDef("low", (std::string)"FF0000", 1.f, false);
    setDef("textShadow", (std::string)"00000", 0.55f, false);
    setDef("spacing", 1.f);
    setDef("textShadow", true);
    setDef("textShadowOffset", 0.003f);
    setDef("textOffsetX", 2.f);
    setDef("showText", true);
    setDef("textLeft", false);
    setDef("bottomUp", false);
    
}

void PotionHUD::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("Potion HUD");
    addSlider("UI Scale", "", "uiscale", 3.f, 0.f, true);
    addToggle("Bottom Up Mode", "New effects appear ontop instead", "bottomUp");
    addSlider("Spacing", "", "spacing", 10.f, 0.0f, true);
    addToggle("Show Text", "", "showText");
    addConditionalSlider(getOps<bool>("showText"), "Text Size", "", "textSize", 0.25f, 0.0f, true);
    addConditionalSlider(getOps<bool>("showText"), "Text Offset X", "", "textOffsetX", 50.f, 0.0f, false);
    addConditionalToggle(getOps<bool>("showText"), "Text to the left", "", "textLeft");
    addConditionalToggle(getOps<bool>("showText"), "Text Shadow", "Displays a shadow under the text", "textShadow");
    addConditionalSlider(getOps<bool>("showText") && getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);
    extraPadding();

    addHeader("Colors");
    addColorPicker("Main Color", "", "main");
    addConditionalColorPicker(getOps<bool>("textShadow"), "Shadow Color", "Text Shadow Color", "textShadow");
    addColorPicker("Effect About to Expire", "", "low");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void PotionHUD::renderText()
{
    auto lowColor = getColor("low");
    auto mainColor = getColor("main");

    int warning_seconds = 5;

    float ospacing = getOps<float>("spacing");

    const bool toTheLeft = getOps<bool>("textLeft");
    const float uiscale = getOps<float>("uiscale");
    const float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
    const float textSize = getOps<float>("textSize") * 1080;
    const float textWidth = Constraints::RelativeConstraint(getOps<float>("textSize"), "height", true);
    const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr)
            if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
                if (getOps<bool>("showText")) {
                    auto scaledPos = PositionUtils::getCustomScreenScaledPos(Vec2<float>{16, 16}, uiscale);
                    float spacing = scaledPos.y * ospacing;

                    float xmodifier = 0.0f;
                    float ymodifier = 0.0f;

                    float xoffset = 0.0f;
                    float yoffset = 0.0f;

                    std::vector<UnifiedMobEffectData> effects = SDK::clientInstance->getLocalPlayer()->getMobEffects();
                    for (const auto& effect : effects) {
                        if (!effect.isValid()) continue;
                        float textX = currentPos.x + (toTheLeft ? 0 : (16 * uiscale * guiscale)) + (getOps<float>("textOffsetX") * guiscale * uiscale) * (toTheLeft ? -1 : 1);
                        float textY = currentPos.y + ymodifier;

                        std::string text = effect.getNameAndTime();
                        std::stringstream ss(text);
                        std::string effectName = effect.getName() + " " + MobEffect::amplifierToString(effect.getAmplifier()); // effect name
                        std::string effectTImeLeft = effect.getTime(); // effect timeLeft

                        std::wstring widestr1 = std::wstring(effectName.begin(), effectName.end());
                        const wchar_t* widecstr1 = widestr1.c_str();

                        std::wstring widestr2 = std::wstring(effectTImeLeft.begin(), effectTImeLeft.end());
                        const wchar_t* widecstr2 = widestr2.c_str();

                        if (getOps<bool>("textShadow")) {
                            D2D_COLOR_F shadowCol = getColor("textShadow");

                            FlarialGUI::FlarialTextWithFont(
                                textX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
                                textY + (Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale) - FlarialGUI::getFlarialTextSize(widecstr1, 0,
                                    16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
                                    textSize * guiscale * uiscale,
                                    DWRITE_FONT_WEIGHT_BOLD, true).y / 4.f,
                                widecstr1,
                                0,
                                16 * uiscale * guiscale,
                                toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
                                textSize * guiscale * uiscale, DWRITE_FONT_WEIGHT_BOLD,
                                shadowCol,
                                true
                            );
                            FlarialGUI::FlarialTextWithFont(
                                textX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
                                textY + (Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale) + FlarialGUI::getFlarialTextSize(widecstr2, 0,
                                    16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
                                    (textSize * guiscale * uiscale) * 0.8f,
                                    DWRITE_FONT_WEIGHT_BOLD, true).y / 1.6f,
                                widecstr2,
                                0,
                                16 * uiscale * guiscale,
                                toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
                                (textSize * guiscale * uiscale) * 0.8f, DWRITE_FONT_WEIGHT_NORMAL,
                                shadowCol,
                                true
                            );
                        }
                        FlarialGUI::FlarialTextWithFont(
                            textX,
                            textY - FlarialGUI::getFlarialTextSize(widecstr1, 0,
                                                                   16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
                                                                   textSize * guiscale * uiscale,
                                                                   DWRITE_FONT_WEIGHT_BOLD, true).y / 4.f,
                            widecstr1, 0,
                            16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
                            textSize * guiscale * uiscale,
                            DWRITE_FONT_WEIGHT_BOLD, effect.duration / 20 <= warning_seconds ? lowColor : mainColor, true);

                        FlarialGUI::FlarialTextWithFont(
                            textX,
                            textY + FlarialGUI::getFlarialTextSize(widecstr2, 0,
                                                                   16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
                                                                   (textSize * guiscale * uiscale) * 0.8f,
                                                                   DWRITE_FONT_WEIGHT_BOLD, true).y / 1.6f,
                            widecstr2, 0,
                            16 * uiscale * guiscale, toTheLeft ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING,
                            (textSize * guiscale * uiscale) * 0.8f,
                            DWRITE_FONT_WEIGHT_NORMAL, effect.duration / 20 <= warning_seconds ? lowColor : mainColor, true);
                        ymodifier += spacing * (getOps<bool>("bottomUp") ? -1 : 1);
                    }
                }
            }
    }
}

void PotionHUD::onRender(RenderEvent& event)
{
    if (ClientInstance::getTopScreenName() == "hud_screen" && this->isEnabled()) {
        float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
        float uiscale = getOps<float>("uiscale");
        float spacing = getOps<float>("spacing");

        Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));
        float s = 16 * uiscale * guiscale;

        float width = s;
        float height = s;

        if (settingperc.x != 0) currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
        else currentPos = Constraints::CenterConstraint(width, height);

        if (ClickGUI::editmenu) {
            // bounding boxes
            FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 23, this->name);
            checkForRightClickAndOpenSettings(currentPos.x, currentPos.y, width, height);
        }

        Vec2<float> vec2;
        // bounding boxes
        vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 23, width, height);


        currentPos.x = vec2.x;
        currentPos.y = vec2.y;

        Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);

        if (ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();

        renderText();
    }

    if (SDK::getCurrentScreen() != "hud_screen") ClickGUI::editmenu = false;
}

void PotionHUD::onRenderPotionHUD(RenderPotionHUDEvent& event)
{
    if (!this->isEnabled()) return;
    event.cancel();
}

void PotionHUD::onSetupAndRender(const SetupAndRenderEvent &event) {
    if (this->isEnabled())
        if (ClientInstance::getTopScreenName() == "hud_screen") {
            Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
            float uiscale = getOps<float>("uiscale");

            if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
                auto ui_icon_scale = uiscale / 2.f;
                float spacing = 16 * uiscale * getOps<float>("spacing");

                float xmodifier = 0.0f;
                float ymodifier = 0.0f;

                auto muirc = event.getMuirc();
                auto effects = SDK::clientInstance->getLocalPlayer()->getMobEffects();
                for (const auto& effect : effects) {
                    if (!effect.isValid()) continue;
                    auto location = effect.getTextureLocation();
                    auto texture = muirc->createTexture(location, false);
                    auto position = Vec2<float>(scaledPos.x + xmodifier, scaledPos.y + ymodifier);
                    auto size = Vec2<float>(16.0f * uiscale, 16.0f * uiscale);
                    static auto uvPos = Vec2<float>(0.f, 0.f);
                    static auto uvSize = Vec2<float>(1.0f, 1.0f);
                    muirc->drawImage(texture, position, size, uvPos, uvSize);
                    static auto color = mce::Color();
                    static auto flushLayer = HashedString("ui_flush");
                    muirc->flushImages(color, 1.0f, flushLayer);
                    ymodifier += spacing * (getOps<bool>("bottomUp") ? -1 : 1);
                }
            }
        }
}
