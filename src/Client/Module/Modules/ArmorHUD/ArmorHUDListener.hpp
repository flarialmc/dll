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
#include <Windows.h>
#include <chrono>

class ArmorHUDListener : public Listener {
public:

    Module *module;
    Vec2<float> currentPos;
    bool enabled = false;
    int durabilities[4][2] = {
        {0,0},
        {0,0},
        {0,0},
        {0,0}
    };
    // TODO: delete testing variables (or adjust and delete)
    Vec2<float> testOffset = Vec2<float>{4,0};
    float testSpacing = 46;



    [[nodiscard]] Vec2<float> convert() const {

        auto e = SDK::clientInstance->guiData;
        Vec2<float> xd = Vec2<float>(e->ScreenSize.x, e->ScreenSize.y);
        Vec2<float> LOL = Vec2<float>(e->ScreenSizeScaled.x, e->ScreenSizeScaled.y);

        return Vec2<float>{currentPos.x * (LOL.x / xd.x), currentPos.y * (LOL.y / xd.y)};
    }
    // TODO: Make it look better
    void renderDurability() {
        if(FlarialGUI::inMenu) return;
        Vec2<float> convert = this->convert();

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);
        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr)
                if (SDK::clientInstance->getLocalPlayer()->playerInventory != nullptr) {

                    if (SDK::clientInstance->getLocalPlayer()->playerInventory->inventory->getItem(
                            SDK::clientInstance->getLocalPlayer()->playerInventory->SelectedSlot)->getItem() !=
                        nullptr) {
                        auto currentItem = SDK::clientInstance->getLocalPlayer()->playerInventory->inventory->getItem(
                                SDK::clientInstance->getLocalPlayer()->playerInventory->SelectedSlot);

                        const std::string text = std::to_string(currentItem->getcount());

                        std::wstring widestr = std::wstring(text.begin(), text.end());

                        const wchar_t *widecstr = widestr.c_str();

                        FlarialGUI::FlarialTextWithFont(currentPos.x + testOffset.x, currentPos.y + testOffset.y,
                                                        widecstr, textWidth * 6.9f,
                                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                                        Constraints::RelativeConstraint(0.12, "height", true),
                                                        DWRITE_FONT_WEIGHT_NORMAL);

                    }

                    float spacing = testSpacing * module->settings.getSettingByName<float>("uiscale")->value;

                    float xmodifier = 0.0f;
                    float ymodifier = 0.0f;

                    float xoffset = 0.0f;
                    float yoffset = 0.0f;

                    if (module->settings.getSettingByName<bool>("vertical")->value) xoffset += spacing;
                    else yoffset += spacing;

                    for (int i = 0; i < 4; i++) {

                        if (module->settings.getSettingByName<bool>("vertical")->value) ymodifier += spacing;
                        else xmodifier += spacing;


                        if (SDK::clientInstance->getLocalPlayer()->getArmor(i)->getItem() != nullptr) {

                            convert = this->convert();


                            const std::string text =
                                    std::to_string(durabilities[i][0]) + "/" + std::to_string(durabilities[i][1]);

                            std::wstring widestr = std::wstring(text.begin(), text.end());

                            const wchar_t *widecstr = widestr.c_str();

                            FlarialGUI::FlarialTextWithFont(
                                    currentPos.x + xmodifier + xoffset + testOffset.x,
                                    currentPos.y + ymodifier + yoffset + testOffset.y, widecstr, textWidth * 6.9f,
                                    textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                    Constraints::RelativeConstraint(0.12, "height", true),
                                    DWRITE_FONT_WEIGHT_NORMAL);
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

        if (SDK::currentScreen != "hud_screen") ClickGUIRenderer::editmenu = false;
    }

    void onSetupAndRender(SetupAndRenderEvent &event) override {
        if (ClientInstance::getTopScreenName() == "hud_screen") {
            auto muirc = event.getMuirc();
            BaseActorRenderContext barc(muirc->screenContext, muirc->clientInstance,
                                        muirc->clientInstance->mcgame);

            Vec2<float> convert = this->convert();
            if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
                if (SDK::clientInstance->getLocalPlayer() != nullptr)
                    if (SDK::clientInstance->getLocalPlayer()->playerInventory != nullptr) {
                        if (SDK::clientInstance->getLocalPlayer()->playerInventory->inventory->getItem(
                                SDK::clientInstance->getLocalPlayer()->playerInventory->SelectedSlot)->getItem() !=
                            nullptr)
                            barc.itemRenderer->renderGuiItemNew(&barc,
                                                                SDK::clientInstance->getLocalPlayer()->playerInventory->inventory->getItem(
                                                                        SDK::clientInstance->getLocalPlayer()->playerInventory->SelectedSlot),
                                                                0, convert.x, convert.y, 1.0f,
                                                                module->settings.getSettingByName<float>(
                                                                        "uiscale")->value,
                                                                false);


                        float spacing = 15 * module->settings.getSettingByName<float>("uiscale")->value;

                        float xmodifier = 0.0f;
                        float ymodifier = 0.0f;

                        for (int i = 0; i < 4; i++) {

                            if (module->settings.getSettingByName<bool>("vertical")->value) ymodifier += spacing;
                            else xmodifier += spacing;

                            if (SDK::clientInstance->getLocalPlayer()->getArmor(i)->getItem() != nullptr) {
                                durabilities[i][1] = SDK::clientInstance->getLocalPlayer()->getArmor(i)->getMaxDamage();
                                durabilities[i][0] = durabilities[i][1] -
                                                     SDK::clientInstance->getLocalPlayer()->getArmor(
                                                             i)->getDamageValue();


                                convert = this->convert();
                                barc.itemRenderer->renderGuiItemNew(&barc,
                                                                    SDK::clientInstance->getLocalPlayer()->getArmor(i),
                                                                    0,
                                                                    convert.x + xmodifier, convert.y + ymodifier, 1.0f,
                                                                    module->settings.getSettingByName<float>(
                                                                            "uiscale")->value, false);
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
