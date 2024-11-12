#pragma once

#include "../Module.hpp"

class PotionHUD : public Module {
private:
    Vec2<float> currentPos{};
    bool enabled = false;

    float testSpacing = 20;
public:

    PotionHUD() : Module("PotionHUD", "Displays your potion effects",
                        IDR_POTION_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &PotionHUD::onRender)
        Listen(this, RenderPotionHUDEvent, &PotionHUD::onRenderPotionHUD)
        Listen(this, SetupAndRenderEvent, &PotionHUD::onSetupAndRender)
        if(FlarialGUI::inMenu){
            FlarialGUI::Notify("To change the position of PotionHUD, Please click " +
                               ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                       "editmenubind")->value + " in the settings tab.");
        }
        Module::onEnable();
    }

    void onDisable() override {

        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 1.0f);

        if (settings.getSettingByName<std::string>("colorMain") == nullptr)
            settings.addSetting("colorMain", (std::string) "FFFFFF");

        if (settings.getSettingByName<bool>("colorMain_rgb") == nullptr) settings.addSetting("colorMain_rgb", false);

        if (settings.getSettingByName<std::string>("colorLow") == nullptr)
            settings.addSetting("colorLow", (std::string) "FF0000");

        if (settings.getSettingByName<bool>("colorLow_rgb") == nullptr) settings.addSetting("colorLow_rgb", false);

        if (settings.getSettingByName<float>("textSize") == nullptr) settings.addSetting("textSize", 0.14f);

    }

    void settingsRender(float settingsOffset) override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(3, toggleX + FlarialGUI::SettingsTextWidth("UI Scale "),
                                           toggleY,
                                           this->settings.getSettingByName<float>("uiscale")->value, 3.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Text size ", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float textSize = FlarialGUI::Slider(1, toggleX + FlarialGUI::SettingsTextWidth("Text size "),
                                           toggleY, this->settings.getSettingByName<float>("textSize")->value, 0.25f,
                                           0.f, false);

        this->settings.getSettingByName<float>("textSize")->value = textSize;

        /* Color Pickers Start*/

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Main color", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(0, toggleX + FlarialGUI::SettingsTextWidth("Main color "),
                                toggleY - Constraints::SpacingConstraint(0.017, textWidth),
                                settings.getSettingByName<std::string>("colorMain")->value,
                                settings.getSettingByName<bool>("colorMain_rgb")->value);

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Effect about to expire", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(2, toggleX + FlarialGUI::SettingsTextWidth("Effect about to expire "), toggleY * 0.99f,
                                settings.getSettingByName<std::string>("colorLow")->value,
                                settings.getSettingByName<bool>("colorLow_rgb")->value);

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("colorMain")->value,
                                      settings.getSettingByName<float>("bgOpacity")->value,
                                      settings.getSettingByName<bool>("colorMain_rgb")->value);
        FlarialGUI::ColorPickerWindow(2, settings.getSettingByName<std::string>("colorLow")->value,
                                      settings.getSettingByName<float>("borderOpacity")->value,
                                      settings.getSettingByName<bool>("colorLow_rgb")->value);
        /* Color Pickers End */
    }

    void renderText() {
        if(FlarialGUI::inMenu && !ClickGUI::editmenu) return;

        auto rgb1 = this->settings.getSettingByName<bool>("colorLow_rgb")->value;
        auto rgb2 = this->settings.getSettingByName<bool>("colorMain_rgb")->value;

        auto rgbColor = FlarialGUI::rgbColor;

        auto lowColor = rgb1 ? rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("colorLow")->value);
        auto mainColor = rgb2 ? rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("colorMain")->value);

        int warning_seconds = 5;

        const float textWidth = Constraints::RelativeConstraint(this->settings.getSettingByName<float>("textSize")->value, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr)
                if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {

                    float spacing = testSpacing;

                    auto scaledPos = PositionUtils::getCustomScreenScaledPos(Vec2<float>{spacing, spacing},
                                                                             this->settings.getSettingByName<float>(
                                                                                     "uiscale")->value);
                    spacing = scaledPos.y;

                    float xmodifier = 0.0f;
                    float ymodifier = 0.0f;

                    float xoffset = 0.0f;
                    float yoffset = 0.0f;

                    std::vector<UnifiedMobEffectData> effects = SDK::clientInstance->getLocalPlayer()->getMobEffects();
                    for(const auto& effect : effects) {
                        if(!effect.isValid()) continue;
                        std::string text = effect.getNameAndTime();
                        std::wstring widestr = std::wstring(text.begin(), text.end());
                        const wchar_t *widecstr = widestr.c_str();
                        FlarialGUI::FlarialTextWithFont(
                                currentPos.x + xmodifier + xoffset,
                                currentPos.y + ymodifier + yoffset, widecstr, textWidth * 6.9f,
                                textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                textWidth,
                                DWRITE_FONT_WEIGHT_NORMAL, effect.duration / 20 <= warning_seconds ? lowColor : mainColor,true);
                        ymodifier += spacing;
                    }
                }
        }
    }

    void onRender(RenderEvent &event) {
        if (ClientInstance::getTopScreenName() == "hud_screen" &&
            this->isEnabled()) {

            float s = Constraints::RelativeConstraint(0.04, "height", true) *
                      this->settings.getSettingByName<float>("uiscale")->value;

            float spacing = Constraints::RelativeConstraint(0.0135, "height", true) *
                            this->settings.getSettingByName<float>("uiscale")->value;

            Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
                                                  this->settings.getSettingByName<float>("percentageY")->value);


            float width = s * 3 + spacing * 2;
            float height = s;

            if (settingperc.x != 0)
        currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
            else
                currentPos = Constraints::CenterConstraint(width, height);

            if (ClickGUI::editmenu) {
                // bounding boxes
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 23);
            }

            Vec2<float> vec2;
            // bounding boxes
            vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 23, width, height);

            currentPos.x = vec2.x;
            currentPos.y = vec2.y;

            Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

            this->settings.setValue("percentageX", percentages.x);
            this->settings.setValue("percentageY", percentages.y);

            if (ClickGUI::editmenu) {
                FlarialGUI::RoundedRect(currentPos.x, currentPos.y, D2D1::ColorF(D2D1::ColorF::White, 0.4f), width, height);
                FlarialGUI::UnsetWindowRect();
            }

            renderText();
        }

        if (SDK::getCurrentScreen() != "hud_screen") ClickGUI::editmenu = false;
    }

    void onRenderPotionHUD(RenderPotionHUDEvent &event) {
        event.cancel();
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        if(this->isEnabled())
            if (ClientInstance::getTopScreenName() == "hud_screen") {
                Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

                if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
                    auto ui_scale = this->settings.getSettingByName<float>("uiscale")->value;
                    auto ui_icon_scale = ui_scale / 2.f;
                    float spacing = 20 * ui_scale;

                    float xmodifier = 0.0f;
                    float ymodifier = 0.0f;

                    auto muirc = event.getMuirc();
                    auto effects = SDK::clientInstance->getLocalPlayer()->getMobEffects();
                    for(const auto& effect : effects) {
                        if(!effect.isValid()) continue;
                        auto location = effect.getTextureLocation();
                        auto texture = muirc->createTexture(location, false);
                        auto position = Vec2<float>(scaledPos.x + xmodifier - 20.0f * ui_scale, scaledPos.y + ymodifier);
                        auto size = Vec2<float>(18.0f * ui_scale, 18.0f * ui_scale);
                        static auto uvPos = Vec2<float>(0.f, 0.f);
                        static auto uvSize = Vec2<float>(1.0f, 1.0f);
                        muirc->drawImage(texture, position, size,uvPos,uvSize);
                        static auto color = mce::Color();
                        static auto flushLayer = HashedString("ui_flush");
                        muirc->flushImages(color, 1.0f, flushLayer);
                        ymodifier += spacing;
                    }
                }
            }
    }
};