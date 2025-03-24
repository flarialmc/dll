#pragma once

#include "../Module.hpp"

class ComboCounter : public Module {
private:
    int Combo = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
public:

    ComboCounter() : Module("Combo", "Keeps track of consecutive hits.", IDR_COMBO_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        Listen(this, AttackEvent, &ComboCounter::onAttack)
        Listen(this, TickEvent, &ComboCounter::onTick)
        Listen(this, RenderEvent, &ComboCounter::onRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, AttackEvent, &ComboCounter::onAttack)
        Deafen(this, TickEvent, &ComboCounter::onTick)
        Deafen(this, RenderEvent, &ComboCounter::onRender)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "Combo: {value}");
        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
    }

    void settingsRender(float settingsOffset) override {

        /* Border Start */

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Module UI");
        this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 2.f);
        this->addToggle("Border", "", settings.getSettingByName<bool>("border")->value);
        this->addConditionalSlider(settings.getSettingByName<bool>("border")->value, "Border Width", "", settings.getSettingByName<float>("borderWidth")->value, 4.f);
        this->addSlider("Rounding", "", this->settings.getSettingByName<float>("rounding")->value);
        this->addToggle("Responsive Rectangle", "Resizes width with text", this->settings.getSettingByName<bool>("responsivewidth")->value);
        this->addSlider("Rotation", "", this->settings.getSettingByName<float>("rotation")->value, 359.f, 0.f, false);
        this->addSlider("Rectangle Width", "", this->settings.getSettingByName<float>("rectwidth")->value, 3.f);
        this->addSlider("Rectangle Height", "", this->settings.getSettingByName<float>("rectheight")->value, 3.f);

        this->extraPadding();

        this->addHeader("Text");
        this->addTextBox("Text Format", "", settings.getSettingByName<std::string>("text")->value);
        this->addSlider("Text Scale", "", this->settings.getSettingByName<float>("textscale")->value, 2.f);
        this->addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, this->settings.getSettingByName<std::string>("textalignment")->value);
        this->addToggle("Reverse Padding X", "", this->settings.getSettingByName<bool>("reversepaddingx")->value);
        this->addToggle("Reverse Padding Y", "", this->settings.getSettingByName<bool>("reversepaddingy")->value);
        this->addSlider("Padding X", "", this->settings.getSettingByName<float>("padx")->value, 1.f, 0.f, false);
        this->addSlider("Padding Y", "", this->settings.getSettingByName<float>("pady")->value, 1.f, 0.f, false);

        this->extraPadding();

        this->addHeader("Colors");
        this->addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value,
                             settings.getSettingByName<float>("bgOpacity")->value,
                             settings.getSettingByName<bool>("bgRGB")->value);
        this->addToggle("Background Shadow", "Displays a shadow under the background", settings.getSettingByName<bool>("rectShadow")->value);
        this->addColorPicker("Shadow Color", "Background Shadow Color", settings.getSettingByName<std::string>("rectShadowCol")->value,
            settings.getSettingByName<float>("rectShadowOpacity")->value,
            settings.getSettingByName<bool>("rectShadowRGB")->value);
        this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("rectShadowOffset")->value, 0.02f, 0.001f);

        this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value,
                             settings.getSettingByName<float>("borderOpacity")->value,
                             settings.getSettingByName<bool>("borderRGB")->value);
        this->addColorPicker("Text Color", "", settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value, settings.getSettingByName<bool>("textRGB")->value);
        this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value, settings.getSettingByName<bool>("bgRGB")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }

    void onAttack(AttackEvent &event) {
        if(std::chrono::high_resolution_clock::now() - last_hit > std::chrono::milliseconds(480)) {
            Combo++;
            last_hit = std::chrono::high_resolution_clock::now();
        }
    }

    void onTick(TickEvent &event) {
        if (!SDK::clientInstance->getLocalPlayer())
            return;

        auto LP = reinterpret_cast<LocalPlayer *>(event.getActor());
        if (LP->getHurtTime() != 0)
            Combo = 0;
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
        if (duration.count() >= 15) Combo = 0;
    }

    void onRender(RenderEvent &event) {
        if (this->isEnabled()) {
            auto comboStr = FlarialGUI::cached_to_string(Combo);
            this->normalRender(8, comboStr);
        }
    }
};