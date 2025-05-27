#pragma once

#include "../Module.hpp"
#include <Events/Render/RenderEvent.hpp>

class InventoryHUD : public Module {
private:
    Vec2<float> currentPos;
    bool enabled = false;
public:

    InventoryHUD() : Module("Inventory HUD", "Displays your inventory\non your HUD",
        IDR_CHESTPLATE_PNG, "") {
        currentPos = Vec2<float>{ 0, 0 };
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &InventoryHUD::onRender)
            Listen(this, SetupAndRenderEvent, &InventoryHUD::onSetupAndRender)
            if (FlarialGUI::inMenu) {
                FlarialGUI::Notify("To change the position of InventoryHUD, Please click " +
                    ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                        "editmenubind")->value + " in the settings tab.");
            }
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &InventoryHUD::onRender)
            Deafen(this, SetupAndRenderEvent, &InventoryHUD::onSetupAndRender)
            Module::onDisable();
    }

    void defaultConfig() override {
        Module::defaultConfig();
        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 2.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 2.0f);
            settings.addSetting("percentageY", 2.0f);
        }
        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.5f);
        if (settings.getSettingByName<std::string>("textColor") == nullptr) settings.addSetting("textColor", (std::string)"#FFFFFF");
        if (settings.getSettingByName<float>("textOpacity") == nullptr) settings.addSetting("textOpacity", 1.0f);
        if (settings.getSettingByName<bool>("textRGB") == nullptr) settings.addSetting("textRGB", false);
        if (settings.getSettingByName<bool>("textShadow") == nullptr) settings.addSetting("textShadow", true);
        if (settings.getSettingByName<std::string>("textShadowCol") == nullptr) settings.addSetting("textShadowCol", (std::string)"#000000");
        if (settings.getSettingByName<float>("textShadowOpacity") == nullptr) settings.addSetting("textShadowOpacity", 0.5f);
        if (settings.getSettingByName<float>("textShadowOffset") == nullptr) settings.addSetting("textShadowOffset", 0.015f);
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addSlider("Size", "", this->settings.getSettingByName<float>("uiscale")->value, 5.f, 0.f, true);
        this->addSlider("Text Size", "", this->settings.getSettingByName<float>("textscale")->value, 2.f, 0.0f, true);
        this->addColorPicker("Text Color", "", settings.getSettingByName<std::string>("textColor")->value,
            settings.getSettingByName<float>("textOpacity")->value,
            settings.getSettingByName<bool>("textRGB")->value);
        this->addSlider("Text Offset X", "", this->settings.getSettingByName<float>("textShadowOpacity")->value, 50.f, 0.0f, false);
        this->addSlider("Text Offset Y", "", this->settings.getSettingByName<float>("textShadowOffset")->value, 50.f, 0.0f, false);
        this->extraPadding();

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onRender(RenderEvent& event) {
        if (ClientInstance::getTopScreenName() == "hud_screen" &&
            this->isEnabled() ||
            ClientInstance::getTopScreenName() == "pause_screen" &&
            this->isEnabled()) {

            float s = Constraints::RelativeConstraint(0.04 * 3.5, "height", true) * // used to be just 0.04
                this->settings.getSettingByName<float>("uiscale")->value;

            float spacing = Constraints::RelativeConstraint(0.012 * 2, "height", true) *
                this->settings.getSettingByName<float>("uiscale")->value;

            Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
                this->settings.getSettingByName<float>("percentageY")->value);

            if (settingperc.x != 0)
                currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - (s * 3 + spacing * 3)), settingperc.y * (MC::windowSize.y - (s)));
            else
                currentPos = Constraints::CenterConstraint(s * 3 + spacing * 3, s);

            if (ClickGUI::editmenu) {
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, s * 3 + spacing * 3, s, 40);
            }

            Vec2<float> vec2;

            vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 40, s * 3 + spacing * 3, s);

            currentPos.x = vec2.x;
            currentPos.y = vec2.y;

            Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, s * 3 + spacing * 3, s);

            this->settings.setValue("percentageX", percentages.x);
            this->settings.setValue("percentageY", percentages.y);

            if (ClickGUI::editmenu) {
                FlarialGUI::UnsetWindowRect();
            }
        }

        if (SDK::getCurrentScreen() != "hud_screen") ClickGUI::editmenu = false;
    }

    void onSetupAndRender(SetupAndRenderEvent& event) {
        if (this->isEnabled() && ClientInstance::getTopScreenName() == "hud_screen") {
            auto muirc = event.getMuirc();
            BaseActorRenderContext barc(muirc->getScreenContext(), muirc->getClientInstance(),
                muirc->getClientInstance()->getMinecraftGame());

            Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

            if (SDK::clientInstance->getLocalPlayer() != nullptr &&
                SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {

                float spacing = 15 * this->settings.getSettingByName<float>("uiscale")->value;
                float uiscale = this->settings.getSettingByName<float>("uiscale")->value;
                float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
                float textScale = this->settings.getSettingByName<float>("textscale")->value * 1080;
                std::string textColor = this->settings.getSettingByName<std::string>("textColor")->value;
                float textOpacity = this->settings.getSettingByName<float>("textOpacity")->value;
                bool textShadow = this->settings.getSettingByName<bool>("textShadow")->value;

                float xmodifier = 0.0f;
                float ymodifier = 0.0f;
                int counter = 0;

                for (int i = 9; i < 36; i++) {
                    auto item = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(i);
                    if (item->getItem() != nullptr) {
                        // Render the item
                        barc.itemRenderer->renderGuiItemNew(&barc, item, 0,
                            scaledPos.x + xmodifier, scaledPos.y + ymodifier,
                            1.0f, uiscale, false);

                        // Render the item count below the item
                        std::string text = FlarialGUI::cached_to_string(item->count);
                        std::wstring wText(text.begin(), text.end());

                        // Position the text below the item, centered
                        float textX = scaledPos.x + xmodifier + (spacing * uiscale * guiscale);
                        float textY = scaledPos.y + ymodifier + (spacing * uiscale * guiscale);

                        D2D1_COLOR_F color = FlarialGUI::HexToColorF(textColor);

                        FlarialGUI::FlarialTextWithFont(
                            textX, textY, wText.c_str(),
                            spacing * uiscale * guiscale, 0,
                            DWRITE_TEXT_ALIGNMENT_CENTER,
                            textScale * uiscale * guiscale,
                            DWRITE_FONT_WEIGHT_NORMAL,
                            color, textShadow
                        );
                    }

                    if (counter < 8) {
                        xmodifier += spacing + 5;
                        counter++;
                    }
                    else {
                        ymodifier += spacing + 5;
                        xmodifier = 0.0f;
                        counter = 0;
                    }
                }
            }
        }
    }
};