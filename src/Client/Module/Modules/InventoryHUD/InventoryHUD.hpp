#pragma once

#include "../Module.hpp"

class InventoryHUD : public Module {
private:
    Vec2<float> currentPos;
    bool enabled = false;
public:

    InventoryHUD() : Module("Inventory HUD", "Displays your inventory\non your HUD",
                            IDR_CHESTPLATE_PNG, "") {
        currentPos = Vec2<float>{0, 0};
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

    void defaultConfig() override { Module::defaultConfig();

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 2.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 3.0f);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onRender(RenderEvent &event) {
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
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, s * 3 + spacing * 3, s, 25);
            }

            Vec2<float> vec2;

            vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 25, s * 3 + spacing * 3, s);

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

    void onSetupAndRender(SetupAndRenderEvent &event) {

        if (this->isEnabled())
            if (ClientInstance::getTopScreenName() == "hud_screen") {
                auto muirc = event.getMuirc();
                BaseActorRenderContext barc(muirc->getScreenContext(), muirc->getClientInstance(),
                                            muirc->getClientInstance()->getMinecraftGame());

                Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

                if (SDK::clientInstance->getLocalPlayer() != nullptr)
                    if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {


                        float spacing = 15 * this->settings.getSettingByName<float>("uiscale")->value;

                        float xmodifier = 0.0f;
                        float ymodifier = 0.0f;

                        int counter = 0;

                        for (int i = 9; i < 36; i++) {
                            if (SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
                                    i)->getItem() != nullptr) {
                                //durabilities[i][1] = SDK::clientInstance->getLocalPlayer()->getArmor(i)->getMaxDamage();
                                //durabilities[i][0] = durabilities[i][1] - SDK::clientInstance->getLocalPlayer()->getArmor(i)->getDamageValue();
                                barc.itemRenderer->renderGuiItemNew(&barc,
                                                                    SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
                                                                            i), 0,
                                                                    scaledPos.x + xmodifier, scaledPos.y + ymodifier,
                                                                    1.0f,
                                                                    this->settings.getSettingByName<float>(
                                                                            "uiscale")->value, false);
                            }
                            if (counter < 8) {
                                xmodifier += spacing + 5;
                                counter++;
                            } else {
                                ymodifier += spacing + 5;
                                xmodifier = 0.0f;
                                counter = 0;
                            }
                        }
                    }
            }
    }
};
