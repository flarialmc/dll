#pragma once

#include "../Module.hpp"

class PotCounter : public Module {
private:
    int pots = 0;
public:
    PotCounter() : Module("Pot Counter", "Counts how much potions are\nin your inventory.",
                          IDR_POTION_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, TickEvent, &PotCounter::onTick)
        Listen(this, RenderEvent, &PotCounter::onRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, TickEvent, &PotCounter::onTick)
        Deafen(this, RenderEvent, &PotCounter::onRender)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value} Pots");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));


        this->addHeader("Main");
        this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 2.0f);
        this->addToggle("Border", "", this->settings.getSettingByName<bool>("border")->value);
        this->addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", this->settings.getSettingByName<bool>(
        "BlurEffect")->value);
        this->addConditionalSlider(this->settings.getSettingByName<bool>(
                "border")->value, "Border Thickness", "", this->settings.getSettingByName<float>("borderWidth")->value, 4.f);
        this->addSlider("Rounding", "Rounding of the rectangle", this->settings.getSettingByName<float>("rounding")->value);

        this->extraPadding();

        this->addHeader("Text");
        this->addTextBox("Format", "", settings.getSettingByName<std::string>("text")->value);
        this->addSlider("Text Scale", "", this->settings.getSettingByName<float>("textscale")->value, 2.0f);
        this->addDropdown("Text Alignment", "",  std::vector<std::string>{"Left", "Center", "Right"}, this->settings.getSettingByName<std::string>("textalignment")->value);
        this->addColorPicker("Color", "Text Color", settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value, settings.getSettingByName<bool>("textRGB")->value);

        this->extraPadding();

        this->addHeader("Colors");
        this->addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value, settings.getSettingByName<bool>("bgRGB")->value);
        this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value, settings.getSettingByName<bool>("borderRGB")->value);

        this->extraPadding();

        this->addHeader("Misc Customizations");

        this->addToggle("Reverse Padding X", "For Text Position",  this->settings.getSettingByName<bool>(
                "reversepaddingx")->value);

        this->addToggle("Reverse Padding Y", "For Text Position",  this->settings.getSettingByName<bool>(
        "reversepaddingy")->value);

        this->addSlider("Padding X", "For Text Position", this->settings.getSettingByName<float>("padx")->value);
        this->addSlider("Padding Y", "For Text Position", this->settings.getSettingByName<float>("pady")->value);

        this->addSlider("Rectangle Width", "", this->settings.getSettingByName<float>("rectwidth")->value, 2.f, 0.001f);
        this->addSlider("Rectangle Height", "", this->settings.getSettingByName<float>("rectheight")->value, 2.f, 0.001f);

        this->addToggle("Responsive Rectangle", "Rectangle resizes with text",  this->settings.getSettingByName<bool>(
"responsivewidth")->value);

        this->addSlider("Rotation", "see for yourself!", this->settings.getSettingByName<float>("rotation")->value, 360.f, 0, false);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    void onTick(TickEvent& event) {
        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                auto potsCount = 0;
                if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
                    auto inventory = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory();

                    if (inventory != nullptr) {
                        for (int i = 0; i < 36; i++) {
                            auto item = inventory->getItem(i);

                            if (item->getItem() != nullptr) {
                                if (item->getItem()->name == "splash_potion") {
                                    potsCount++;
                                }
                            }
                        }
                    }

                    pots = potsCount;
                }
            }
        }
    }

    void onRender(RenderEvent& event) {
        auto potsStr = std::to_string(pots);
        this->normalRender(14, potsStr);
    }
};