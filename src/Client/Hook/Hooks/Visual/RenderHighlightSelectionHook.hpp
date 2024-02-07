#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK/Client/Render/ScreenContext.hpp"

class RenderHighlightSelectionHook : public Hook
{
private:

    static void RenderHighlightSelectionCallback(LevelRendererPlayer* _this, ScreenContext& screenContext, Block const& block, BlockSource& region, Vec3<int> const& pos) {

        auto mod = ModuleManager::getModule("BlockOutline");
        bool enabled = mod ? mod->settings.getSettingByName<bool>("enabled")->value : false;

        static auto color = Memory::offsetFromSig(Memory::findSig("0F 28 05 ? ? ? ? 0F 11 45 ? 0F 11 00 C6 40 10 ? 45 38 96"), 3);

        if (enabled) {
            D2D1_COLOR_F color2;
            if (mod->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
            else color2 = FlarialGUI::HexToColorF(mod->settings.getSettingByName<std::string>("color")->value);

            MCCColor c;
            c.r = color2.r;
            c.g = color2.g;
            c.b = color2.b;
            c.a = mod->settings.getSettingByName<float>("colorOpacity")->value;

            Memory::patchBytes((void*)color, &c, 0x10);
        }

        func_original(_this, screenContext, block, region, pos);

        if (enabled) {
            static MCCColor black(0.65f, 0.65f, 0.65f, 1.f);

            Memory::patchBytes((void*)color, &black, 0x10);
        }

    }

public:
    typedef void(__thiscall* RenderHighlightSelectionOriginal)(LevelRendererPlayer*, ScreenContext&, Block const&, BlockSource&, Vec3<int> const&);
    static inline RenderHighlightSelectionOriginal func_original = nullptr;

    RenderHighlightSelectionHook() : Hook("renderHighlightSelection Hook", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 8B F9 4C 89 4C 24") {}

    void enableHook() override {

        this->autoHook(RenderHighlightSelectionCallback, (void**)&func_original);

    }
};


