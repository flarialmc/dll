#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../Utils/Render/PositionUtils.hpp"

class ArmorHUD : public Module {
private:
    Vec2<float> currentPos;
    bool enabled = false;
    int durabilities[6][2] = {{0,0}};
    // TODO: delete testing variables (or adjust and delete)
    Vec2<float> testOffset = Vec2<float>{0,0};
    float testSpacing = 15;
public:

    ArmorHUD() : Module("ArmorHUD", "Displays the armor you're\ncurrently wearing.",
                        IDR_CHESTPLATE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        if(FlarialGUI::inMenu){
            std::string s = "To change the position of ArmorHUD, Please click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("editmenubind")->value + " in the settings tab.";
            std::cout << s << std::endl;
            FlarialGUI::Notify(s);
        }
        Listen(this, RenderEvent, &ArmorHUD::onRender)
        Listen(this, SetupAndRenderEvent, &ArmorHUD::onSetupAndRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &ArmorHUD::onRender)
        Deafen(this, SetupAndRenderEvent, &ArmorHUD::onSetupAndRender)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 2.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }
        if (settings.getSettingByName<bool>("show_offhand") == nullptr) settings.addSetting("show_offhand", true);
        if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);
        if (settings.getSettingByName<bool>("durability_left") == nullptr) settings.addSetting("durability_left", false);
        if (settings.getSettingByName<bool>("percent") == nullptr) settings.addSetting("percent", false);
        if (settings.getSettingByName<bool>("color") == nullptr) settings.addSetting("color", false);
        if (settings.getSettingByName<bool>("showdurability") == nullptr) settings.addSetting("showdurability", false);

        if (settings.getSettingByName<std::string>("colorMain") == nullptr)
            settings.addSetting("colorMain", (std::string) "FFFFFF");

        if (settings.getSettingByName<bool>("colorMain_rgb") == nullptr) settings.addSetting("colorMain_rgb", false);

        if (settings.getSettingByName<std::string>("colorFull") == nullptr)
            settings.addSetting("colorFull", (std::string) "40FF00");

        if (settings.getSettingByName<bool>("colorFull_rgb") == nullptr) settings.addSetting("colorFull_rgb", false);

        if (settings.getSettingByName<std::string>("colorLow") == nullptr)
            settings.addSetting("colorLow", (std::string) "FF0000");

        if (settings.getSettingByName<bool>("colorLow_rgb") == nullptr) settings.addSetting("colorLow_rgb", false);

        if (settings.getSettingByName<float>("textSize") == nullptr) settings.addSetting("textSize", 0.12f);

    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));


        this->addHeader("Main");

        this->addSlider("Size", "", this->settings.getSettingByName<float>("uiscale")->value, 5.f, 0.f, true);
        this->addToggle("Vertical ArmorHUD", "To switch between a vertical or horizontal layout", this->settings.getSettingByName<bool>("vertical")->value);
        if(this->settings.getSettingByName<bool>("vertical")->value){
            this->addToggle("Durability to the left", "", this->settings.getSettingByName<bool>("durability_left")->value);
        }

        this->addToggle("Show offhand item", "", this->settings.getSettingByName<bool>("show_offhand")->value);

        this->addToggle("Change Color", "", this->settings.getSettingByName<bool>("color")->value);

        this->extraPadding();
        this->addHeader("Text");

        this->addToggle("Show Durability", "", this->settings.getSettingByName<bool>("showdurability")->value);
        this->addConditionalSlider(this->settings.getSettingByName<bool>("showdurability")->value, "Text Size", "Text size of Durability", this->settings.getSettingByName<float>("textSize")->value, 0.25f, 0.0f, true);
        this->addToggle("Show Durability in %", "", this->settings.getSettingByName<bool>("percent")->value);

        this->extraPadding();
        this->addHeader("Colors");

        this->addColorPicker("Main", "", settings.getSettingByName<std::string>("colorMain")->value,
                                      settings.getSettingByName<float>("bgOpacity")->value,
                                      settings.getSettingByName<bool>("colorMain_rgb")->value);

        this->addColorPicker("Full", "", settings.getSettingByName<std::string>("colorFull")->value,
                                      settings.getSettingByName<float>("textOpacity")->value,
                                      settings.getSettingByName<bool>("colorFull_rgb")->value);

        this->addColorPicker("Low", "", settings.getSettingByName<std::string>("colorLow")->value,
                                      settings.getSettingByName<float>("borderOpacity")->value,
                                      settings.getSettingByName<bool>("colorLow_rgb")->value);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    // TODO: Make it look better
    void renderDurability() {
        if(FlarialGUI::inMenu) return;

        auto rgb = this->settings.getSettingByName<bool>("colorFull_rgb")->value;
        auto rgb1 = this->settings.getSettingByName<bool>("colorLow_rgb")->value;
        auto rgb2 = this->settings.getSettingByName<bool>("colorMain_rgb")->value;

        auto rgbColor = FlarialGUI::rgbColor;

        auto showOffhand = this->settings.getSettingByName<bool>("show_offhand")->value;

        auto fullColor = rgb ? rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("colorFull")->value);
        auto lowColor = rgb1 ? rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("colorLow")->value);
        auto mainColor = rgb2 ? rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("colorMain")->value);

        const float textWidth = Constraints::RelativeConstraint(this->settings.getSettingByName<float>("textSize")->value, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr)
                if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {

                    auto vertical = this->settings.getSettingByName<bool>("vertical")->value;
                    auto durability_left = this->settings.getSettingByName<bool>("durability_left")->value;

                    float spacing = testSpacing;

                    auto scaledPos = PositionUtils::getCustomScreenScaledPos(Vec2<float>{spacing, spacing},
                                                                             this->settings.getSettingByName<float>(
                                                                                     "uiscale")->value);
                    spacing = vertical ? scaledPos.y : scaledPos.x;

                    float xmodifier = 0.0f;
                    float ymodifier = 0.0f;

                    for (int i = 2; i < 6; i++) {

                        if (vertical) ymodifier += spacing;
                        else xmodifier += spacing;

                        if (SDK::clientInstance->getLocalPlayer()->getArmor(i - 2)->getItem() != nullptr) {

                            std::string text;

                            if (durabilities[i][1] != 0) { // for some servers with custom items with max durability of 0
                                if (this->settings.getSettingByName<bool>("percent")->value)
                                    text = FlarialGUI::cached_to_string((int) std::round(
                                            (float) durabilities[i][0] / (float) durabilities[i][1] * 100)) + "%";
                                else
                                    text = FlarialGUI::cached_to_string(durabilities[i][0]) + "/" +
                                           FlarialGUI::cached_to_string(durabilities[i][1]);


                                std::wstring widestr = std::wstring(text.begin(), text.end());

                                const wchar_t *widecstr = widestr.c_str();

                                D2D1_COLOR_F color = mainColor;

                                if (this->settings.getSettingByName<bool>("color")->value) {
                                    if (std::round((float) durabilities[i][0] / (float) durabilities[i][1] * 100) <=
                                        15) {
                                        color = lowColor;
                                    } else {
                                        color = fullColor;
                                    }
                                }

                                float spacingX = vertical ? (durabilities[i][1] != 0 ? spacing : 0) : 0;
                                if (durability_left) spacingX *= -1;
                                float spacingY = vertical ? 0 : (durabilities[i][1] != 0 ? spacing : 0);

                                FlarialGUI::FlarialTextWithFont(
                                        currentPos.x + xmodifier + spacingX + testOffset.x,
                                        currentPos.y + ymodifier + spacingY + testOffset.y, widecstr, textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        textWidth,
                                        DWRITE_FONT_WEIGHT_NORMAL, color, true);
                            }
                        }
                    }

                    if (SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
                            SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot())->getItem() !=
                        nullptr) {
                        auto currentItem = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
                                SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot());

                        std::string text;

                        if (durabilities[0][1] != 0) {
                            if (this->settings.getSettingByName<bool>("percent")->value)
                                text = FlarialGUI::cached_to_string((int) std::round(
                                        (float) durabilities[0][0] / (float) durabilities[0][1] * 100)) + "%";
                            else
                                text = FlarialGUI::cached_to_string(durabilities[0][0]) + "/" + FlarialGUI::cached_to_string(durabilities[0][1]);
                        } else {
                            text = FlarialGUI::cached_to_string(currentItem->getcount());
                        }

                        std::wstring widestr = std::wstring(text.begin(), text.end());

                        const wchar_t *widecstr = widestr.c_str();

                        D2D1_COLOR_F color = mainColor;

                        if (this->settings.getSettingByName<bool>("color")->value && durabilities[0][1] != 0) {
                            if (std::round((float) durabilities[0][0] / (float) durabilities[0][1] * 100) <= 15) {
                                color = lowColor;
                            } else {
                                color = fullColor;
                            }
                        }

                        if (vertical) ymodifier += spacing;
                        else xmodifier += spacing;

                        float spacingX = vertical ? (durabilities[0][1] != 0 ? spacing : 0) : 0;
                        if (durability_left) spacingX *= -1;
                        float spacingY = vertical ? 0 : (durabilities[0][1] != 0 ? spacing : 0);

                        FlarialGUI::FlarialTextWithFont(currentPos.x + testOffset.x + xmodifier + spacingX,
                                                        currentPos.y + testOffset.y + ymodifier + spacingY,
                                                        widecstr, textWidth * 6.9f,
                                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                                        textWidth,
                                                        DWRITE_FONT_WEIGHT_NORMAL, color, true);

                    }
                    if (showOffhand) {
                        if (SDK::clientInstance->getLocalPlayer()->getOffhandSlot() != nullptr &&
                            SDK::clientInstance->getLocalPlayer()->getOffhandSlot()->getItem() != nullptr) {

                            auto offhandItem = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();

                            std::string text;

                            if (durabilities[1][1] != 0) {
                                if (this->settings.getSettingByName<bool>("percent")->value)
                                    text = FlarialGUI::cached_to_string((int) std::round(
                                            (float) durabilities[1][0] / (float) durabilities[1][1] * 100)) + "%";
                                else
                                    text = FlarialGUI::cached_to_string(durabilities[1][0]) + "/" +
                                           FlarialGUI::cached_to_string(durabilities[1][1]);
                            } else {
                                text = FlarialGUI::cached_to_string(offhandItem->getcount());
                            }

                            std::wstring widestr = std::wstring(text.begin(), text.end());

                            const wchar_t *widecstr = widestr.c_str();

                            D2D1_COLOR_F color = mainColor;

                            if (this->settings.getSettingByName<bool>("color")->value && durabilities[1][1] != 0) {
                                if (std::round((float) durabilities[1][0] / (float) durabilities[1][1] * 100) <= 15) {
                                    color = lowColor;
                                } else {
                                    color = fullColor;
                                }
                            }

                            if (vertical) ymodifier += spacing;
                            else xmodifier += spacing;

                            float spacingX = vertical ? (durabilities[1][1] != 0 ? spacing : 0) : 0;
                            if (durability_left) spacingX *= -1;
                            float spacingY = vertical ? 0 : (durabilities[1][1] != 0 ? spacing : 0);

                            FlarialGUI::FlarialTextWithFont(currentPos.x + testOffset.x + xmodifier + spacingX,
                                                            currentPos.y + testOffset.y + ymodifier + spacingY,
                                                            widecstr, textWidth * 6.9f,
                                                            textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                                            textWidth,
                                                            DWRITE_FONT_WEIGHT_NORMAL, color, true);

                        }
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

            float width;
            float height;

            if (!this->settings.getSettingByName<bool>("vertical")->value) {
                width = s * 3 + spacing * 3;
                height = s;
            }else{
                width = s;
                height = s * 3 + spacing * 3;
            }

            if (settingperc.x != 0)
        currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
            else
                currentPos = Constraints::CenterConstraint(width, height);

            if (ClickGUI::editmenu) {
                // bounding boxes
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 18);
            }

            Vec2<float> vec2;
            // bounding boxes
            vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 18, width, height);


            currentPos.x = vec2.x;
            currentPos.y = vec2.y;

            Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

            this->settings.setValue("percentageX", percentages.x);
            this->settings.setValue("percentageY", percentages.y);

            if (ClickGUI::editmenu) {
                FlarialGUI::UnsetWindowRect();
            }

            if(this->settings.getSettingByName<bool>("showdurability")->value)
                renderDurability();
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

                float spacing = 15 * this->settings.getSettingByName<float>("uiscale")->value;

                float xmodifier = 0.0f;
                float ymodifier = 0.0f;

                if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
                    if (SDK::clientInstance->getLocalPlayer() != nullptr)
                        if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
                            for (int i = 2; i < 6; i++) {

                                if (this->settings.getSettingByName<bool>("vertical")->value) ymodifier += spacing;
                                else xmodifier += spacing;

                                auto armorSlot = SDK::clientInstance->getLocalPlayer()->getArmor(i-2);

                                if (armorSlot->getItem() != nullptr) {

                                    auto maxDamage = armorSlot->getMaxDamage();
                                    auto durabilityLeft = maxDamage - armorSlot->getDamageValue();

                                    durabilities[i][1] = maxDamage;
                                    durabilities[i][0] = durabilityLeft;

                                    barc.itemRenderer->renderGuiItemNew(&barc,
                                                                        armorSlot,
                                                                        0,
                                                                        scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                        this->settings.getSettingByName<float>(
                                                                                "uiscale")->value, false);

                                    if(armorSlot->isEnchanted()) {
                                        barc.itemRenderer->renderGuiItemNew(&barc,
                                                                            armorSlot,
                                                                            0,
                                                                            scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                            this->settings.getSettingByName<float>(
                                                                                    "uiscale")->value,
                                                                            true);
                                    }
                                }
                            }

                            if (SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
                                    SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot())->getItem() !=
                                nullptr) {
                                auto item = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
                                        SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot());

                                auto maxDamage = item->getMaxDamage();
                                auto durabilityLeft = maxDamage - item->getDamageValue();

                                durabilities[0][1] = maxDamage;
                                durabilities[0][0] = durabilityLeft;

                                if (this->settings.getSettingByName<bool>("vertical")->value) ymodifier += spacing;
                                else xmodifier += spacing;

                                barc.itemRenderer->renderGuiItemNew(&barc,
                                                                    item,
                                                                    0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                    this->settings.getSettingByName<float>(
                                                                            "uiscale")->value,
                                                                    false);

                                if(item->isEnchanted()) {
                                    barc.itemRenderer->renderGuiItemNew(&barc,
                                                                        item,
                                                                        0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                        this->settings.getSettingByName<float>(
                                                                                "uiscale")->value,
                                                                        true);
                                }
                            }

                            auto showOffhand = this->settings.getSettingByName<bool>("show_offhand")->value;
                            if(showOffhand) {
                                if (SDK::clientInstance->getLocalPlayer()->getOffhandSlot() != nullptr &&
                                    SDK::clientInstance->getLocalPlayer()->getOffhandSlot()->getItem() != nullptr) {
                                    auto item = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();

                                    auto maxDamage = item->getMaxDamage();
                                    auto durabilityLeft = maxDamage - item->getDamageValue();

                                    durabilities[1][1] = maxDamage;
                                    durabilities[1][0] = durabilityLeft;

                                    if (this->settings.getSettingByName<bool>("vertical")->value) ymodifier += spacing;
                                    else xmodifier += spacing;

                                    barc.itemRenderer->renderGuiItemNew(&barc,
                                                                        item,
                                                                        0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                        this->settings.getSettingByName<float>(
                                                                                "uiscale")->value,
                                                                        false);

                                    if (item->isEnchanted()) {
                                        barc.itemRenderer->renderGuiItemNew(&barc,
                                                                            item,
                                                                            0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                            this->settings.getSettingByName<float>(
                                                                                    "uiscale")->value,
                                                                            true);
                                    }
                                }
                            }
                        }
                }
            }
    }
};