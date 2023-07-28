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

    Module* module;
    Vec2<float> currentPos;
    bool enabled = false;


    void onLocalTick(TickEvent& event) override {

    }

    Vec2<float> convert() {

        auto e = SDK::clientInstance->guiData;
        Vec2<float> xd = Vec2<float>(e->ScreenSize.x, e->ScreenSize.y);
        Vec2<float> LOL = Vec2<float>(e->ScreenSizeScaled.x, e->ScreenSizeScaled.y);

        return {currentPos.x * (LOL.x / xd.x), currentPos.y * (LOL.y / xd.y)};
    }

    void onRender(RenderEvent& event) override {


        if(SDK::clientInstance->getTopScreenName() == "hud_screen" && module->settings.getSettingByName<bool>("enabled")->value || SDK::clientInstance->getTopScreenName() == "pause_screen" && module->settings.getSettingByName<bool>("enabled")->value) {

            if(!enabled && ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value) {
                FlarialGUI::Notify("To change the position of ArmorHUD, Please click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("editmenubind")->value);
                enabled = true;
            }

            float s = Constraints::RelativeConstraint(0.1, "height", true) * module->settings.getSettingByName<float>("uiscale")->value;

            Vec2<float> settingperc = Vec2<float>(module->settings.getSettingByName<float>("percentageX")->value,
            module->settings.getSettingByName<float>("percentageY")->value);

            if (settingperc.x != 0)
                currentPos = Vec2<float>(settingperc.x * MC::windowSize.x,
                                         settingperc.y * MC::windowSize.y);
            else
                currentPos = Constraints::CenterConstraint(s, s);

            FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, s, s, 15);

            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 15, s, s);


            currentPos.x = vec2.x;
            currentPos.y = vec2.y;

            currentPos = currentPos;

            Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y);

            module->settings.setValue("percentageX", percentages.x);
            module->settings.setValue("percentageY", percentages.y);

            FlarialGUI::UnsetWindowRect();
        } else {
            enabled = false;
        }
    }

    void onSetupAndRender(SetupAndRenderEvent& event) override {

        if(SDK::clientInstance->getTopScreenName() == "hud_screen" && module->settings.getSettingByName<bool>("enabled")->value) {

            BaseActorRenderContext barc(event.muirc->screenContext, event.muirc->clientInstance, event.muirc->clientInstance->mcgame);

            Vec2<float> convert = this->convert();

            if(SDK::clientInstance->getLocalPlayer()->playerInventory->inventory->getItem(0)->getItem() != nullptr)
            barc.itemRenderer->renderGuiItemNew(&barc, SDK::clientInstance->getLocalPlayer()->playerInventory->inventory->getItem(0), 0, convert.x, convert.y, 1.0f, module->settings.getSettingByName<float>("uiscale")->value, false);

            float s = Constraints::RelativeConstraint(0.1, "height", true) * module->settings.getSettingByName<float>("uiscale")->value;

            float spacing = Constraints::SpacingConstraint(1.2f, s);

            Vec2<float> oldPos = currentPos;

            for (int i = 0; i < 4; i++) {

                if(module->settings.getSettingByName<bool>("vertical")->value) currentPos.x += spacing;
                else currentPos.y += spacing;

            }

            currentPos = oldPos;
        }
    }

public:
    explicit ArmorHUDListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
        this->currentPos = Vec2<float>(0, 0);
    }

};