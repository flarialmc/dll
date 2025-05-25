#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Block/BlockLegacy.hpp"
#include "../../../../SDK/Client/Block/BlockSource.hpp"
#include "../../../../SDK/Client/Block/Block.hpp"

class Waila : public Module {
public:
    std::string lookingAt;

    Waila() : Module("Waila", "Shows what you are looking at.", IDR_WAILA_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, SetupAndRenderEvent, &Waila::onSetupAndRender)
        Listen(this, RenderEvent, &Waila::onRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, SetupAndRenderEvent, &Waila::onSetupAndRender)
        Deafen(this, RenderEvent, &Waila::onRender)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string)"{value}");
        this->settings.getSettingByName<bool>("responsivewidth")->value = true;
        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
        if (this->settings.getSettingByName<bool>("advanced") == nullptr) this->settings.addSetting("advanced", false);
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
        this->addToggle("Advanced Mode", "", this->settings.getSettingByName<bool>("advanced")->value);
        this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 2.0f);
        this->addToggle("Border", "", this->settings.getSettingByName<bool>("border")->value);
        this->addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", this->settings.getSettingByName<bool>("BlurEffect")->value);
        this->addConditionalSlider(this->settings.getSettingByName<bool>("border")->value, "Border Thickness", "", this->settings.getSettingByName<float>("borderWidth")->value, 4.f);
        this->addSlider("Rounding", "Rounding of the rectangle", this->settings.getSettingByName<float>("rounding")->value);
        this->extraPadding();
        
        this->addHeader("Text");
        this->addTextBox("Format", "", settings.getSettingByName<std::string>("text")->value);
        this->addSlider("Text Scale", "", this->settings.getSettingByName<float>("textscale")->value, 2.0f);
        this->addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"},
            this->settings.getSettingByName<std::string>("textalignment")->value);
        this->addColorPicker("Color", "Text Color", settings.getSettingByName<std::string>("textColor")->value,
            settings.getSettingByName<float>("textOpacity")->value,
            settings.getSettingByName<bool>("textRGB")->value);
        this->addToggle("Text Shadow", "Displays a shadow under the text", settings.getSettingByName<bool>("textShadow")->value);
        this->addColorPicker("Shadow Color", "Text Shadow Color", settings.getSettingByName<std::string>("textShadowCol")->value,
            settings.getSettingByName<float>("textShadowOpacity")->value,
            settings.getSettingByName<bool>("textShadowRGB")->value);
        this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("textShadowOffset")->value, 0.02f, 0.001f);

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

        this->extraPadding();

        this->addHeader("Misc Customizations");
        this->addToggle("Reverse Padding X", "For Text Position", this->settings.getSettingByName<bool>("reversepaddingx")->value);
        this->addToggle("Reverse Padding Y", "For Text Position", this->settings.getSettingByName<bool>("reversepaddingy")->value);
        this->addSlider("Padding X", "For Text Position", this->settings.getSettingByName<float>("padx")->value);
        this->addSlider("Padding Y", "For Text Position", this->settings.getSettingByName<float>("pady")->value);
        this->addSlider("Rectangle Width", "", this->settings.getSettingByName<float>("rectwidth")->value, 2.f, 0.001f);
        this->addSlider("Rectangle Height", "", this->settings.getSettingByName<float>("rectheight")->value, 2.f, 0.001f);
        this->addToggle("Responsive Rectangle", "Rectangle resizes with text", this->settings.getSettingByName<bool>("responsivewidth")->value);
        this->addSlider("Rotation", "see for yourself!", this->settings.getSettingByName<float>("rotation")->value, 360.f, 0, false);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }


    void onSetupAndRender(SetupAndRenderEvent& event) { 
        if (!SDK::clientInstance->getLocalPlayer()) return;
        if (!SDK::clientInstance->getLocalPlayer()->getLevel()) return;
        if (!SDK::clientInstance->getBlockSource()) return;
        HitResult result = SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult();

        BlockPos pos = { result.blockPos.x,
                         result.blockPos.y ,
                         result.blockPos.z};
        BlockSource* blockSource = SDK::clientInstance->getBlockSource();
        try {
            BlockLegacy* block = blockSource->getBlock(pos)->getBlockLegacy();
            if (!block) return;
            try {

                if (!this->settings.getSettingByName<bool>("advanced")->value) lookingAt = block->getName();
                else lookingAt = block->getNamespace();
            }
            catch (const std::exception& e) { Logger::error("Failed to get block name: {}", e.what()); }
        }
        catch (const std::exception& e) {
            Logger::error("Failed to get block: {}", e.what());
        }
    }

    void onRender(RenderEvent& event) {
        if (this->isEnabled() && lookingAt != "air") {
            this->normalRender(32, lookingAt);
        }
    }
};
