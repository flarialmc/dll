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

class ArmorHUDListener : public Listener {
public:

    Module *module;
    Vec2<float> currentPos;
    bool enabled = false;
    int durabilities[5][2] = {
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0}
    };
    // TODO: delete testing variables (or adjust and delete)
    Vec2<float> testOffset = Vec2<float>{4,0};
    float testSpacing = 15;

    [[nodiscard]] Vec2<float> convert() const {

        auto e = SDK::clientInstance->guiData;
        Vec2<float> xd = Vec2<float>(e->ScreenSize.x, e->ScreenSize.y);
        Vec2<float> LOL = Vec2<float>(e->ScreenSizeScaled.x, e->ScreenSizeScaled.y);

        return Vec2<float>{currentPos.x * (LOL.x / xd.x), currentPos.y * (LOL.y / xd.y)};
    }

    [[nodiscard]] Vec2<float> convert(Vec2<float> pos) {

        auto e = SDK::clientInstance->guiData;
        Vec2<float> LOL = Vec2<float>(e->ScreenSize.x, e->ScreenSize.y);
        Vec2<float> xd = Vec2<float>(e->ScreenSizeScaled.x, e->ScreenSizeScaled.y);
        float scale = module->settings.getSettingByName<float>("uiscale")->value;
        return Vec2<float>{pos.x * (LOL.x / xd.x) * scale, pos.y * (LOL.y / xd.y) * scale};
    }

    // TODO: Make it look better
    void renderDurability() { }

    void onRender(RenderEvent &event) override { }

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
                            nullptr) {
                            auto item = SDK::clientInstance->getLocalPlayer()->playerInventory->inventory->getItem(
                                    SDK::clientInstance->getLocalPlayer()->playerInventory->SelectedSlot);
                            durabilities[0][1] = item->getMaxDamage();
                            durabilities[0][0] = durabilities[0][1] - item->getDamageValue();
                            barc.itemRenderer->renderGuiItemNew(&barc,
                                                                item,
                                                                0, convert.x, convert.y, 1.0f,
                                                                module->settings.getSettingByName<float>(
                                                                        "uiscale")->value,
                                                                false);
                        }


                        float spacing = 15 * module->settings.getSettingByName<float>("uiscale")->value;

                        float xmodifier = 0.0f;
                        float ymodifier = 0.0f;

                        for (int i = 1; i < 5; i++) {

                            if (module->settings.getSettingByName<bool>("vertical")->value) ymodifier += spacing;
                            else xmodifier += spacing;

                            if (SDK::clientInstance->getLocalPlayer()->getArmor(i-1)->getItem() != nullptr) {
                                durabilities[i][1] = SDK::clientInstance->getLocalPlayer()->getArmor(i-1)->getMaxDamage();
                                durabilities[i][0] = durabilities[i][1] -
                                                     SDK::clientInstance->getLocalPlayer()->getArmor(
                                                             i-1)->getDamageValue();


                                convert = this->convert();
                                barc.itemRenderer->renderGuiItemNew(&barc,
                                                                    SDK::clientInstance->getLocalPlayer()->getArmor(i-1),
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

    explicit ArmorHUDListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
        this->currentPos = Vec2<float>(0, 0);
    }

};
