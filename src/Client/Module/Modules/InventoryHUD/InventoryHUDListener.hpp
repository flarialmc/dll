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

class InventoryHUDListener : public Listener {
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

    Vec2<float> testOffset = Vec2<float>{0,0};



    [[nodiscard]] Vec2<float> convert() const {

        auto e = SDK::clientInstance->guiData;
        Vec2<float> xd = Vec2<float>(e->ScreenSize.x, e->ScreenSize.y);
        Vec2<float> LOL = Vec2<float>(e->ScreenSizeScaled.x, e->ScreenSizeScaled.y);

        return Vec2<float>{currentPos.x * (LOL.x / xd.x), currentPos.y * (LOL.y / xd.y)};
    }

    void onRender(RenderEvent &event) override {
        if (ClientInstance::getTopScreenName() == "hud_screen" &&
            module->isEnabled() ||
            ClientInstance::getTopScreenName() == "pause_screen" &&
            module->isEnabled()) {

            float s = Constraints::RelativeConstraint(0.04 * 3.5, "height", true) * // used to be just 0.04
                      module->settings.getSettingByName<float>("uiscale")->value;

            float spacing = Constraints::RelativeConstraint(0.012 * 2, "height", true) *
                            module->settings.getSettingByName<float>("uiscale")->value;

            Vec2<float> settingperc = Vec2<float>(module->settings.getSettingByName<float>("percentageX")->value,
                                                  module->settings.getSettingByName<float>("percentageY")->value);

            if (settingperc.x != 0)
                currentPos = Vec2<float>(settingperc.x * MC::windowSize.x,
                                         settingperc.y * MC::windowSize.y);
            else
                currentPos = Constraints::CenterConstraint(s * 3 + spacing * 3, s);

            if (ClickGUIRenderer::editmenu) {
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, s * 3 + spacing * 3, s, -18);
            }

            Vec2<float> vec2;

            vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, -18, s * 3 + spacing * 3, s);

            currentPos.x = vec2.x;
            currentPos.y = vec2.y;

            Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y);

            module->settings.setValue("percentageX", percentages.x);
            module->settings.setValue("percentageY", percentages.y);

            if (ClickGUIRenderer::editmenu) {
                FlarialGUI::RoundedRect(currentPos.x, currentPos.y, D2D1::ColorF(D2D1::ColorF::White, 0.2f), s * 3 + spacing * 3, s);
                FlarialGUI::UnsetWindowRect();
            }
        }

        if (SDK::currentScreen != "hud_screen") ClickGUIRenderer::editmenu = false;
    }

    void onSetupAndRender(SetupAndRenderEvent &event) override {
        if (ClientInstance::getTopScreenName() == "hud_screen") {
            auto muirc = event.getMuirc();
            BaseActorRenderContext barc(muirc->screenContext, muirc->clientInstance,
                                        muirc->clientInstance->mcgame);

            Vec2<float> convert = this->convert();

            if (SDK::clientInstance->getLocalPlayer() != nullptr)
                if (SDK::clientInstance->getLocalPlayer()->playerInventory != nullptr) {


                    float spacing = 15 * module->settings.getSettingByName<float>("uiscale")->value;

                    float xmodifier = 0.0f;
                    float ymodifier = 0.0f;

                    int counter = 0;

                    for (int i = 9; i < 36; i++) {
                        if (SDK::clientInstance->getLocalPlayer()->playerInventory->getinventory()->getItem(i)->getItem() != nullptr) {
                            //durabilities[i][1] = SDK::clientInstance->getLocalPlayer()->getArmor(i)->getMaxDamage();
                            //durabilities[i][0] = durabilities[i][1] - SDK::clientInstance->getLocalPlayer()->getArmor(i)->getDamageValue();
                            convert = this->convert();
                            barc.itemRenderer->renderGuiItemNew(&barc,
                                                                SDK::clientInstance->getLocalPlayer()->playerInventory->getinventory()->getItem(
                                                                        i), 0,
                                                                convert.x + xmodifier, convert.y + ymodifier, 1.0f,
                                                                module->settings.getSettingByName<float>(
                                                                        "uiscale")->value, false);
                        }
                        if(counter < 8) {
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

public:
    explicit InventoryHUDListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
        this->currentPos = Vec2<float>(0, 0);
    }

};
