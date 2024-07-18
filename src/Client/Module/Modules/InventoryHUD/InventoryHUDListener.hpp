#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"

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
