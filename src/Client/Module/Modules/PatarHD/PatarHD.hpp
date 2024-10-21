#pragma once

#include "../Module.hpp"


class PatarHD : public Module {
private:
    float x = 0;
    float y = 0;
    float xv = 1;
    float yv = 1;
public:

    PatarHD() : Module("PatarHD", "Who is this now?", IDR_SKULL_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &PatarHD::onRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &PatarHD::onRender)
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<bool>("dvdmode") == nullptr)
            settings.addSetting("dvdmode", true);
        if (settings.getSettingByName<float>("xveloc") == nullptr)
            settings.addSetting("xveloc", 1.0f);
        if (settings.getSettingByName<float>("yveloc") == nullptr)
            settings.addSetting("yveloc", 0.69f);
        if (settings.getSettingByName<float>("scale") == nullptr)
            settings.addSetting("scale", 1.0f);
    }

    void settingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(7.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(1.0f, "height"));

        if (FlarialGUI::Toggle(0, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "dvdmode")->value))
            this->settings.getSettingByName<bool>("dvdmode")->value = !this->settings.getSettingByName<bool>(
                    "dvdmode")->value;

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, L"DVD Mode",
                                        textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"X Velocity", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(0, toggleX + FlarialGUI::SettingsTextWidth("X Velocity "),
                                           toggleY, this->settings.getSettingByName<float>("xveloc")->value, 25.0f);

        this->settings.getSettingByName<float>("xveloc")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Y Velocity", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(1, toggleX + FlarialGUI::SettingsTextWidth("Y Velocity "),
                                     toggleY, this->settings.getSettingByName<float>("yveloc")->value, 25.0f);

        this->settings.getSettingByName<float>("yveloc")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Scale", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(2, toggleX + FlarialGUI::SettingsTextWidth("Scale "),
                                     toggleY, this->settings.getSettingByName<float>("scale")->value, 5.0f);

        this->settings.getSettingByName<float>("scale")->value = percent;

        FlarialGUI::UnsetScrollView();

    }

    void onRender(RenderEvent &event) {
        if (this->isEnabled() &&
            ClientInstance::getTopScreenName() == "hud_screen") {
            float s = Constraints::RelativeConstraint(0.35, "height", true) *
                      this->settings.getSettingByName<float>("scale")->value;
            if (this->settings.getSettingByName<bool>("dvdmode")->value) {
                FlarialGUI::image(IDR_PATAR_JPG,
                                  D2D1::RectF(x, y, x + s, y + s), "JPG");

                x += this->settings.getSettingByName<float>("xveloc")->value * xv;
                y += this->settings.getSettingByName<float>("yveloc")->value * yv;

                if (x >= MC::windowSize.x - s) xv = -1;
                if (x < 0) xv = 1;
                if (y >= MC::windowSize.y - s) yv = -1;
                if (y < 0) yv = 1;
            } else {
                Vec2<float> center = Constraints::CenterConstraint(s, s);
                FlarialGUI::image(IDR_PATAR_JPG,
                                  D2D1::RectF(center.x, center.y, center.x + s, center.y + s));
            }
        }
    }
};