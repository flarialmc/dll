#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"
#include "../../../../Utils/Render/PositionUtils.hpp"
#include <Windows.h>
#include <chrono>

class ArmorHUDListener : public Listener {
public:

    Module *module;
    Vec2<float> currentPos;
    bool enabled = false;
    int durabilities[6][2] = {{0,0}};
    // TODO: delete testing variables (or adjust and delete)
    Vec2<float> testOffset = Vec2<float>{0,0};
    float testSpacing = 15;

    // TODO: Make it look better
    void renderDurability() {
        if(FlarialGUI::inMenu) return;

        auto rgb = module->settings.getSettingByName<bool>("colorFull_rgb")->value;
        auto rgb1 = module->settings.getSettingByName<bool>("colorLow_rgb")->value;
        auto rgb2 = module->settings.getSettingByName<bool>("colorMain_rgb")->value;

        auto rgbColor = FlarialGUI::rgbColor;

        auto showOffhand = module->settings.getSettingByName<bool>("show_offhand")->value;

        auto fullColor = rgb ? rgbColor : FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("colorFull")->value);
        auto lowColor = rgb1 ? rgbColor : FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("colorLow")->value);
        auto mainColor = rgb2 ? rgbColor : FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("colorMain")->value);

        const float textWidth = Constraints::RelativeConstraint(module->settings.getSettingByName<float>("textSize")->value, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr)
                if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {

                    auto vertical = module->settings.getSettingByName<bool>("vertical")->value;
                    auto durability_left = module->settings.getSettingByName<bool>("durability_left")->value;

                    float spacing = testSpacing;

                    auto scaledPos = PositionUtils::getCustomScreenScaledPos(Vec2<float>{spacing, spacing},
                                                                             module->settings.getSettingByName<float>(
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
                                if (module->settings.getSettingByName<bool>("percent")->value)
                                    text = std::to_string((int) std::round(
                                            (float) durabilities[i][0] / (float) durabilities[i][1] * 100)) + "%";
                                else
                                    text = std::to_string(durabilities[i][0]) + "/" +
                                           std::to_string(durabilities[i][1]);


                                std::wstring widestr = std::wstring(text.begin(), text.end());

                                const wchar_t *widecstr = widestr.c_str();

                                D2D1_COLOR_F color = mainColor;

                                if (module->settings.getSettingByName<bool>("color")->value) {
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
                            if (module->settings.getSettingByName<bool>("percent")->value)
                                text = std::to_string((int) std::round(
                                        (float) durabilities[0][0] / (float) durabilities[0][1] * 100)) + "%";
                            else
                                text = std::to_string(durabilities[0][0]) + "/" + std::to_string(durabilities[0][1]);
                        } else {
                            text = std::to_string(currentItem->getcount());
                        }

                        std::wstring widestr = std::wstring(text.begin(), text.end());

                        const wchar_t *widecstr = widestr.c_str();

                        D2D1_COLOR_F color = mainColor;

                        if (module->settings.getSettingByName<bool>("color")->value && durabilities[0][1] != 0) {
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
                                if (module->settings.getSettingByName<bool>("percent")->value)
                                    text = std::to_string((int) std::round(
                                            (float) durabilities[1][0] / (float) durabilities[1][1] * 100)) + "%";
                                else
                                    text = std::to_string(durabilities[1][0]) + "/" +
                                           std::to_string(durabilities[1][1]);
                            } else {
                                text = std::to_string(offhandItem->getcount());
                            }

                            std::wstring widestr = std::wstring(text.begin(), text.end());

                            const wchar_t *widecstr = widestr.c_str();

                            D2D1_COLOR_F color = mainColor;

                            if (module->settings.getSettingByName<bool>("color")->value && durabilities[1][1] != 0) {
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

    void onRender(RenderEvent &event) override {
        if (ClientInstance::getTopScreenName() == "hud_screen" &&
            module->isEnabled()) {

            float s = Constraints::RelativeConstraint(0.04, "height", true) *
                      module->settings.getSettingByName<float>("uiscale")->value;

            float spacing = Constraints::RelativeConstraint(0.0135, "height", true) *
                            module->settings.getSettingByName<float>("uiscale")->value;

            Vec2<float> settingperc = Vec2<float>(module->settings.getSettingByName<float>("percentageX")->value,
                                                  module->settings.getSettingByName<float>("percentageY")->value);

            float width;
            float height;

            if (!module->settings.getSettingByName<bool>("vertical")->value) {
                width = s * 3 + spacing * 3;
                height = s;
            }else{
                width = s;
                height = s * 3 + spacing * 3;
            }

            if (settingperc.x != 0)
                currentPos = Vec2<float>(settingperc.x * MC::windowSize.x,
                                         settingperc.y * MC::windowSize.y);
            else
                currentPos = Constraints::CenterConstraint(width, height);

            if (ClickGUIRenderer::editmenu) {
                // bounding boxes
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 18);
            }

            Vec2<float> vec2;
            // bounding boxes
            vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 18, width, height);

            currentPos.x = vec2.x;
            currentPos.y = vec2.y;

            Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y);

            module->settings.setValue("percentageX", percentages.x);
            module->settings.setValue("percentageY", percentages.y);

            if (ClickGUIRenderer::editmenu) {
                FlarialGUI::RoundedRect(currentPos.x, currentPos.y, D2D1::ColorF(D2D1::ColorF::White, 0.4f), width, height);
                FlarialGUI::UnsetWindowRect();
            }

            if(module->settings.getSettingByName<bool>("showdurability")->value)
                renderDurability();
        }

        if (SDK::getCurrentScreen() != "hud_screen") ClickGUIRenderer::editmenu = false;
    }

    void onSetupAndRender(SetupAndRenderEvent &event) override {
        if (this->module->isEnabled())
            if (ClientInstance::getTopScreenName() == "hud_screen") {
                auto muirc = event.getMuirc();
                BaseActorRenderContext barc(muirc->getScreenContext(), muirc->getClientInstance(),
                                            muirc->getClientInstance()->getMinecraftGame());

                Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

                float spacing = 15 * module->settings.getSettingByName<float>("uiscale")->value;

                float xmodifier = 0.0f;
                float ymodifier = 0.0f;

                if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
                    if (SDK::clientInstance->getLocalPlayer() != nullptr)
                        if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
                            for (int i = 2; i < 6; i++) {

                                if (module->settings.getSettingByName<bool>("vertical")->value) ymodifier += spacing;
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
                                                                        module->settings.getSettingByName<float>(
                                                                                "uiscale")->value, false);

                                    if(armorSlot->isEnchanted()) {
                                        barc.itemRenderer->renderGuiItemNew(&barc,
                                                                            armorSlot,
                                                                            0,
                                                                            scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                            module->settings.getSettingByName<float>(
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

                                if (module->settings.getSettingByName<bool>("vertical")->value) ymodifier += spacing;
                                else xmodifier += spacing;

                                barc.itemRenderer->renderGuiItemNew(&barc,
                                                                    item,
                                                                    0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                    module->settings.getSettingByName<float>(
                                                                            "uiscale")->value,
                                                                    false);

                                if(item->isEnchanted()) {
                                    barc.itemRenderer->renderGuiItemNew(&barc,
                                                                        item,
                                                                        0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                        module->settings.getSettingByName<float>(
                                                                                "uiscale")->value,
                                                                        true);
                                }
                            }

                            auto showOffhand = module->settings.getSettingByName<bool>("show_offhand")->value;
                            if(showOffhand) {
                                if (SDK::clientInstance->getLocalPlayer()->getOffhandSlot() != nullptr &&
                                    SDK::clientInstance->getLocalPlayer()->getOffhandSlot()->getItem() != nullptr) {
                                    auto item = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();

                                    auto maxDamage = item->getMaxDamage();
                                    auto durabilityLeft = maxDamage - item->getDamageValue();

                                    durabilities[1][1] = maxDamage;
                                    durabilities[1][0] = durabilityLeft;

                                    if (module->settings.getSettingByName<bool>("vertical")->value) ymodifier += spacing;
                                    else xmodifier += spacing;

                                    barc.itemRenderer->renderGuiItemNew(&barc,
                                                                        item,
                                                                        0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                        module->settings.getSettingByName<float>(
                                                                                "uiscale")->value,
                                                                        false);

                                    if (item->isEnchanted()) {
                                        barc.itemRenderer->renderGuiItemNew(&barc,
                                                                            item,
                                                                            0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
                                                                            module->settings.getSettingByName<float>(
                                                                                    "uiscale")->value,
                                                                            true);
                                    }
                                }
                            }
                        }
                }
            }
    }

public:
    explicit ArmorHUDListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
        this->currentPos = Vec2<float>(0, 0);
    }

};
