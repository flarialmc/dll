#pragma once

#include "../Module.hpp"


class GuiScale : public Module {
public:
    GuiScale() : Module("MC GUI Scale", "Change your GUI Scale beyond\nMinecraft's restrictions.",
                        IDR_SCALE_PNG, "") {

        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &GuiScale::onRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &GuiScale::onRender)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("guiscale") == nullptr) settings.addSetting("guiscale", 2.0f);
    }

    void settingsRender(float settingsOffset) override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);


        float percent = FlarialGUI::Slider(4, toggleX + FlarialGUI::SettingsTextWidth("UI Scale "),
                                           toggleY, this->settings.getSettingByName<float>("guiscale")->value, 4.0f);

        this->settings.getSettingByName<float>("guiscale")->value = percent;

    }

    void onRender(RenderEvent &event) {
        if (SDK::getCurrentScreen() == "hud_screen") {
            if (SDK::clientInstance->getGuiData() != nullptr) {
                if (this->settings.getSettingByName<bool>("enabled")->value) {
                    float percent = this->settings.getSettingByName<float>("guiscale")->value;
                    auto guiData = SDK::clientInstance->getGuiData();
                    guiData->GuiScale = percent;
                    guiData->ScreenSizeScaled = Vec2{
                            guiData->ScreenSize.x * 1 / percent,
                            guiData->ScreenSize.y * 1 / percent
                    };
                    guiData->scalingMultiplier = 1 / percent;
                }
            }
        }
    }
};
